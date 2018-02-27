#include "MainToolbar.h"
#include "ConfigExtension.h"
#include "ConfigPath.h"
#include "Widgets/HVLine.h"
#include "Widgets/Layout/Raw.h"
#include "Widgets/Toolbar/VerticalGroup.h"
#include "Widgets/Toolbar/HorizontalGroup.h"
#include "Widgets/Toolbar/InlineToolButton.h"
#include "Widgets/Toolbar/BigToolButton.h"
#include "ImageProcessing/Utils/ColoredPixmap.h"
#include <QToolButton>
#include <QHBoxLayout>

namespace phobos {
namespace {
config::ConfigPath const basePath("navigationBar");
config::ConfigPath const iconPath = basePath("icon");
} // unnamed namespace

MainToolbar::MainToolbar(QWidget *parent) :
  QWidget(parent), _hidden(false)
{
  auto const separatorMargin = config::qualified(basePath("config")("separatorMargin"), 2u);
  _layout = new QHBoxLayout;
  _layout->setContentsMargins(separatorMargin, 0, separatorMargin, 0);
  _layout->setSpacing(config::qualified(basePath("config")("spacing"), 2u));

  setupFileGroup();
  setupViewGroup();
  setupSeriesGroup();
  setupSelectGroup();
  setupProcessGroup();
  setupEnhanceGroup();
  _layout->addStretch();
  setupHelpGroup();

  QHBoxLayout *mainLayout = widgets::layout::makeRaw<QHBoxLayout>();
  mainLayout->addLayout(_layout);
  setupHideButton(mainLayout);

  setLayout(mainLayout);
  setMinimumHeight(config::qualified(basePath("config")("minimumHeight"), 0u));
}

void MainToolbar::setContentsMargins(int left, int top, int right, int bottom) const
{
  layout()->setContentsMargins(left, top, right, bottom);
}

void MainToolbar::setHidden(bool hide)
{
  if (hide == _hidden)
    return;

  _hidden = hide;

  for (int i = 0; i < _layout->count(); ++i)
    if (QWidget* wgt = _layout->itemAt(i)->widget())
      wgt->setVisible(!hide);

  _hideButton->setIcon(iprocess::utils::coloredPixmap(iconPath(hide ? "showNavigation" : "hideNavigation"), QSize(64, 64)));
}

void MainToolbar::setGroupVisible(std::string const& group, bool visible)
{
  auto const it = _groups.find(group);
  if (it != _groups.end())
    it->second->setVisible(visible);
}

QToolButton* MainToolbar::getButton(std::string const& key) const
{
  auto const it = _buttons.find(key);
  if (it == _buttons.end())
    return nullptr;
  return it->second;
}

QToolButton* MainToolbar::registerButton(std::string const& key, QToolButton *button)
{
  _buttons.emplace(key, button);
  return button;
}

namespace {
QWidget* makeGroupSeparator()
{
  widgets::HVLine *line = new widgets::HVLine(Qt::Vertical);
  line->setMinimumWidth(config::qualified(basePath("config")("separatorMargin"), 2u));
  return line;
}

QWidget* addGroupWithSeparator(widgets::toolbar::detail::HorizontalGroupBase *group,
                               std::string const& name,
                               QHBoxLayout *layout,
                               std::map<std::string, QWidget*> & mapping,
                               std::set<std::string> & groupNames)
{
  QWidget *separator = makeGroupSeparator();
  group->setVisibleBuddy(separator);

  layout->addWidget(group);
  layout->addWidget(separator);

  mapping.emplace(name, group);
  groupNames.emplace(name);

  return group;
}
} // unnamed namespace

QWidget* MainToolbar::setupFileGroup()
{
  using namespace widgets::toolbar;

  QToolButton *importButton = registerButton("fileImport", new BigToolButton(tr("Import"), iconPath("fileImport")));

  NamedHorizontalGroup *group = NamedHorizontalGroup::create(tr("File"), importButton);
  return addGroupWithSeparator(group, "file", _layout, _groups, _groupNames);
}

QWidget* MainToolbar::setupViewGroup()
{
  using namespace widgets::toolbar;

  QToolButton *allSeriesButton = registerButton("viewAllSeries", new InlineToolButton(tr("All series"), iconPath("viewAllSeries")));
  QToolButton *numSeriesButton = registerButton("viewSingleSeries", new InlineToolButton(tr("Single series"), iconPath("viewSingleSeries")));
  QToolButton *rowSeriesButton = registerButton("viewScrollable", new InlineToolButton(tr("Scrollable"), iconPath("viewScrollable")));
  QToolButton *previewButton = registerButton("viewFullscreenPreview", new BigToolButton(tr("Fullscreen\npreview"), iconPath("viewFullscreenPreview")));
  QToolButton *labButton = registerButton("viewLaboratory", new BigToolButton(tr("Enhance\nphotos"), iconPath("viewLaboratory")));
  QToolButton *detailsButton = registerButton("viewPhotoDetails", new BigToolButton(tr("Photo\ndetails"), iconPath("viewPhotoDetails")));

  NamedHorizontalGroup *group = NamedHorizontalGroup::create(tr("View"),
        VerticalGroup::create(allSeriesButton, numSeriesButton, rowSeriesButton),
        previewButton,
        labButton,
        detailsButton);

  return addGroupWithSeparator(group, "view", _layout, _groups, _groupNames);
}

QWidget* MainToolbar::setupSeriesGroup()
{
  using namespace widgets::toolbar;

  QToolButton *previousSeries = registerButton("seriesPrevious", new InlineToolButton(tr("Previous"), iconPath("seriesPrevious")));
  QToolButton *nextSeries = registerButton("seriesNext", new InlineToolButton(tr("Next"), iconPath("seriesNext")));

  NamedHorizontalGroup *group = NamedHorizontalGroup::create(tr("Series"),
        VerticalGroup::create(nextSeries, previousSeries));

  return addGroupWithSeparator(group, "series", _layout, _groups, _groupNames);
}

QWidget* MainToolbar::setupSelectGroup()
{
  using namespace widgets::toolbar;

  QToolButton *bestButton = registerButton("selectBest", new BigToolButton(tr("Best"), iconPath("selectBest")));
  QToolButton *allButton = registerButton("selectAll", new InlineToolButton(tr("All"), iconPath("selectAll")));
  QToolButton *invertButton = registerButton("selectInvert", new InlineToolButton(tr("Invert"), iconPath("selectInvert")));
  QToolButton *clearButton = registerButton("selectClear", new InlineToolButton(tr("Clear"), iconPath("selectClear")));

  NamedHorizontalGroup *group = NamedHorizontalGroup::create(tr("Select"),
        bestButton,
        VerticalGroup::create(allButton, invertButton, clearButton));

  return addGroupWithSeparator(group, "select", _layout, _groups, _groupNames);
}

QWidget* MainToolbar::setupProcessGroup()
{
  using namespace widgets::toolbar;

  QToolButton *deleteButton = registerButton("processDelete", new BigToolButton(tr("Delete"), iconPath("processDelete")));
  QToolButton *moveButton = registerButton("processMove", new InlineToolButton(tr("Move"), iconPath("processMove")));
  QToolButton *copyButton = registerButton("processCopy", new InlineToolButton(tr("Copy"), iconPath("processCopy")));
  QToolButton *renameButton = registerButton("processRename", new InlineToolButton(tr("Rename"), iconPath("processRename")));

  NamedHorizontalGroup *group = NamedHorizontalGroup::create(tr("Process"),
        deleteButton,
        VerticalGroup::create(moveButton, copyButton, renameButton));

  return addGroupWithSeparator(group, "process", _layout, _groups, _groupNames);
}

QWidget* MainToolbar::setupEnhanceGroup()
{
  using namespace widgets::toolbar;

  QToolButton *whiteBalance = registerButton("enhanceWhiteBalance", new BigToolButton(tr("White\nbalance"), iconPath("enhanceWhiteBalance")));

  NamedHorizontalGroup *group = NamedHorizontalGroup::create(tr("Enhance"),
        whiteBalance);

  return addGroupWithSeparator(group, "enhance", _layout, _groups, _groupNames);
}

QWidget* MainToolbar::setupHelpGroup()
{
  using namespace widgets::toolbar;

  QToolButton *licenseButton = registerButton("helpLicense", new InlineToolButton(iconPath("helpLicense")));

  HorizontalGroup *group = HorizontalGroup::create(licenseButton);

  return addGroupWithSeparator(group, "help", _layout, _groups, _groupNames);
}

void MainToolbar::setupHideButton(QBoxLayout *target)
{
  using namespace widgets::toolbar;

  _hideButton = new InlineToolButton(iconPath("hideNavigation"));
  _hideButton->setIconSize(config::qSize(basePath("config")("hideButtonSize"), QSize(12, 12)));
  QObject::connect(_hideButton, &QToolButton::clicked, [this](){ setHidden(!_hidden); });
  target->addWidget(_hideButton, 0, Qt::AlignBottom);
}

} // namespace phobos
