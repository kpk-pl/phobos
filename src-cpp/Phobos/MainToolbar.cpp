#include "MainToolbar.h"
#include "ConfigExtension.h"
#include "ConfigPath.h"
#include "Widgets/HVLine.h"
#include "Widgets/Layout/Raw.h"
#include "Widgets/Toolbar/VerticalGroup.h"
#include "Widgets/Toolbar/HorizontalGroup.h"
#include "Widgets/Toolbar/InlineToolButton.h"
#include "Widgets/Toolbar/BigToolButton.h"
#include "Widgets/Toolbar/BigMenuToolButton.h"
#include "ImageProcessing/Utils/ColoredPixmap.h"
#include <QToolButton>
#include <QHBoxLayout>

namespace phobos {
namespace {
config::ConfigPath const basePath("navigationBar");
config::ConfigPath const buttonPath = basePath("button");
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
  _fixedSizeHint = sizeHint().height();
  setFixedHeight(_fixedSizeHint);
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

  for (auto const& group : _groups)
    group.second.widget->setVisible(hide ? false : group.second.visibility);

  if (hide)
  {
    setMinimumHeight(0);
    setMaximumHeight(QWIDGETSIZE_MAX);
  }
  else
    setFixedHeight(_fixedSizeHint);

  _hideButton->setIcon(iprocess::utils::coloredPixmap(buttonPath(hide ? "showNavigation" : "hideNavigation"), QSize(64, 64)));
}

void MainToolbar::setGroupVisible(std::string const& group, bool visible)
{
  auto const it = _groups.find(group);
  if (it != _groups.end())
  {
    if (!_hidden)
      it->second.widget->setVisible(visible);

    it->second.visibility = visible;
  }
}

widgets::toolbar::Signal const* MainToolbar::getSignal(std::string const& key) const
{
  auto const it = _buttonSignals.find(key);
  if (it == _buttonSignals.end())
    return nullptr;
  return it->second;
}

namespace {
QWidget* makeGroupSeparator()
{
  widgets::HVLine *line = new widgets::HVLine(Qt::Vertical);
  line->setMinimumWidth(config::qualified(basePath("config")("separatorMargin"), 2u));
  return line;
}
} // unnamed namespace

QWidget* MainToolbar::addGroupWithSeparator(widgets::toolbar::detail::HorizontalGroupBase *group,
                                            std::string const& name,
                                            QHBoxLayout *layout)
{
  QWidget *separator = makeGroupSeparator();
  group->setVisibleBuddy(separator);

  layout->addWidget(group);
  layout->addWidget(separator);

  _groups.emplace(name, GroupInfo{group, true});
  _groupNames.emplace(name);

  return group;
}

namespace {
struct ButtonCreator
{
  ButtonCreator(std::map<std::string, widgets::toolbar::Signal const*> & signalMap) : signalMap(signalMap)
  {}

  template<typename T>
  T* make(std::string const& name, QString const& textLabel, QString const& tooltip) const
  {
    T* button = new T(textLabel, buttonPath(name));
    configure(button, name, tooltip);
    return button;
  }

  template<typename T>
  T* make(std::string const& name, QString const& tooltip) const
  {
    T* button = new T(buttonPath(name));
    configure(button, name, tooltip);
    return button;
  }

private:
  void configure(widgets::toolbar::ToolButton *button, std::string const& name, QString const& tooltip) const
  {
    if (auto const& shortcut = config::qualified<std::string>(buttonPath(name)("shortcut")))
    {
      button->setShortcut(QString::fromStdString(*shortcut));
      button->setToolTip(tooltip + " (<b>" + button->shortcut().toString() + "</b>)");
    }
    else
    {
      button->setToolTip(tooltip);
    }

    signalMap.emplace(name, button->getSignal("clicked"));
  }

  std::map<std::string, widgets::toolbar::Signal const*> & signalMap;
};
} // unnamed namespace

QWidget* MainToolbar::setupFileGroup()
{
  using namespace widgets::toolbar;
  ButtonCreator creator(_buttonSignals);

  NamedHorizontalGroup *group = NamedHorizontalGroup::create(tr("File"),
      creator.make<BigToolButton>("fileImport", tr("Import"), tr("Import photos"))
  );

  return addGroupWithSeparator(group, "file", _layout);
}

QWidget* MainToolbar::setupViewGroup()
{
  using namespace widgets::toolbar;
  ButtonCreator creator(_buttonSignals);

  NamedHorizontalGroup *group = NamedHorizontalGroup::create(tr("View"),
      VerticalGroup::create(
          creator.make<InlineToolButton>("viewAllSeries", tr("All series"), tr("Show all series on one page")),
          creator.make<InlineToolButton>("viewSingleSeries", tr("Single series"), tr("Show side by side photos from one series")),
          creator.make<InlineToolButton>("viewScrollable", tr("Scrollable"), tr("Show one series with zoomed photos on a single page with horizontal scrolling capability"))
      ),
      creator.make<BigToolButton>("viewFullscreenPreview", tr("Fullscreen\npreview"), tr("Open a separate preview dialog with a single fullscreen photo")),
      creator.make<BigToolButton>("viewLaboratory", tr("Enhance\nphotos"), tr("Switch to enhancements and editing workspace")),
      creator.make<BigToolButton>("viewPhotoDetails", tr("Photo\ndetails"), tr("Show details for selected photo"))
  );

  return addGroupWithSeparator(group, "view", _layout);
}

QWidget* MainToolbar::setupSeriesGroup()
{
  using namespace widgets::toolbar;
  ButtonCreator creator(_buttonSignals);

  NamedHorizontalGroup *group = NamedHorizontalGroup::create(tr("Series"),
      VerticalGroup::create(
          creator.make<InlineToolButton>("seriesPrevious", tr("Previous"), tr("Jump to previous series")),
          creator.make<InlineToolButton>("seriesNext", tr("Next"), tr("Jump to next series"))
      )
  );

  return addGroupWithSeparator(group, "series", _layout);
}

QWidget* MainToolbar::setupSelectGroup()
{
  using namespace widgets::toolbar;
  ButtonCreator creator(_buttonSignals);

  NamedHorizontalGroup *group = NamedHorizontalGroup::create(tr("Select"),
      creator.make<BigToolButton>("selectBest", tr("Best"), tr("Automatically select best photos in each series")),
      VerticalGroup::create(
          creator.make<InlineToolButton>("selectAll", tr("All"), tr("Select all photos")),
          creator.make<InlineToolButton>("selectInvert", tr("Invert"), tr("Invert selection")),
          creator.make<InlineToolButton>("selectClear", tr("Clear"), tr("Clear selection"))
      )
  );

  return addGroupWithSeparator(group, "select", _layout);
}

QWidget* MainToolbar::setupProcessGroup()
{
  using namespace widgets::toolbar;
  ButtonCreator creator(_buttonSignals);

  NamedHorizontalGroup *group = NamedHorizontalGroup::create(tr("Process"),
      creator.make<BigToolButton>("processDelete", tr("Delete"), tr("Delete selected files from hard drive")),
      VerticalGroup::create(
          creator.make<InlineToolButton>("processMove", tr("Move"), tr("Move selected files")),
          creator.make<InlineToolButton>("processCopy", tr("Copy"), tr("Copy selected files")),
          creator.make<InlineToolButton>("processRename", tr("Rename"), tr("Rename selected files"))
      )
  );

  return addGroupWithSeparator(group, "process", _layout);
}

QWidget* MainToolbar::setupEnhanceGroup()
{
  using namespace widgets::toolbar;
  ButtonCreator creator(_buttonSignals);

  BigMenuToolButton *save = creator.make<BigMenuToolButton>("enhanceSave", tr("Save enhanced photo"));
  save->addOption(tr("Save as"), "saveAs");

  _buttonSignals.emplace("enhanceSave.saveAs", save->getSignal("saveAs"));

  NamedHorizontalGroup *group = NamedHorizontalGroup::create(tr("Enhance"),
      creator.make<BigToolButton>("enhanceWhiteBalance", tr("White\nbalance"), tr("Automatically adjust white balance")),
      save
  );

  return addGroupWithSeparator(group, "enhance", _layout);
}

QWidget* MainToolbar::setupHelpGroup()
{
  using namespace widgets::toolbar;
  ButtonCreator creator(_buttonSignals);

  HorizontalGroup *group = HorizontalGroup::create(
      creator.make<InlineToolButton>("helpLicense", tr("Help"))
  );

  return addGroupWithSeparator(group, "help", _layout);
}

void MainToolbar::setupHideButton(QBoxLayout *target)
{
  using namespace widgets::toolbar;

  _hideButton = new InlineToolButton(buttonPath("hideNavigation"));
  _hideButton->setIconSize(config::qSize(basePath("config")("hideButtonSize"), QSize(12, 12)));
  QObject::connect(_hideButton, &QToolButton::clicked, [this](){ setHidden(!_hidden); });
  target->addWidget(_hideButton, 0, Qt::AlignBottom);
}

} // namespace phobos
