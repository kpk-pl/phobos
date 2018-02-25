#include "MainToolbar.h"
#include "ConfigExtension.h"
#include "ConfigPath.h"
#include "Widgets/HVLine.h"
#include "ImageProcessing/Utils/ColoredPixmap.h"

#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

namespace phobos {
namespace {
config::ConfigPath const basePath("navigationBar");
} // unnamed namespace

// TODO: Need to move navigation to a separate group
// Need to add another commands to ViewStack handleSwitchView to handle single photo change
// This way, next photo and prev photo can be handled along with next series and prev series
// This widget may be hidden on allSeriesView as not really necessary
MainToolbar::MainToolbar(QWidget *parent) :
  QWidget(parent), _hidden(false)
{
  auto const separatorMargin = config::qualified(basePath("separatorMargin"), 2u);
  _layout = new QHBoxLayout;
  _layout->setContentsMargins(separatorMargin, 0, separatorMargin, 0);
  _layout->setSpacing(config::qualified(basePath("spacing"), 2u));

  setupFileGroup();
  setupViewGroup();
  setupSelectGroup();
  setupProcessGroup();
  _layout->addStretch();
  setupHelpGroup();

  QHBoxLayout *mainLayout = new QHBoxLayout;
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);

  mainLayout->addLayout(_layout);
  setupHideButton(mainLayout);

  setLayout(mainLayout);
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

  _hideButton->setIcon(iprocess::utils::coloredPixmap(basePath(hide ? "showNavigation" : "hideNavigation"), QSize(64, 64)));
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
class GroupLabel : public QLabel
{
public:
  explicit GroupLabel(QString const& label, QWidget *parent = nullptr) : QLabel(label, parent)
  {
    setAlignment(Qt::AlignCenter | Qt::AlignBottom);
  }
};

class ToolButton : public QToolButton
{
public:
  explicit ToolButton(config::ConfigPath const& configPath)
  {
    setIcon(iprocess::utils::coloredPixmap(configPath, QSize(64, 64)));
    setAutoRaise(true);
  }

  explicit ToolButton(QString const& label, config::ConfigPath const& configPath) :
    ToolButton(configPath)
  {
    setText(label);
  }
};

class BigToolButton : public ToolButton
{
public:
  explicit BigToolButton(QString const& label, config::ConfigPath const& configPath) :
    ToolButton(label, configPath)
  {
    setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    setIconSize(config::qSize(basePath("buttonSize"), QSize(40, 40)));
    setContentsMargins(0, 0, 0, 0);
  }
};

class InlineToolButton : public ToolButton
{
public:
  explicit InlineToolButton(QString const& label, config::ConfigPath const& configPath) :
    ToolButton(label, configPath)
  {
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setIconSize(config::qSize(basePath("inlineButtonSize"), QSize(14, 14)));
    setContentsMargins(0, 0, 0, 0);
  }

  explicit InlineToolButton(config::ConfigPath const& configPath) :
    ToolButton(configPath)
  {
    setToolButtonStyle(Qt::ToolButtonIconOnly);
    setIconSize(config::qSize(basePath("inlineButtonSize"), QSize(14, 14)));
    setContentsMargins(0, 0, 0, 0);
  }
};

QWidget* makeGroupSeparator()
{
  widgets::HVLine *line = new widgets::HVLine(Qt::Vertical);
  line->setMinimumWidth(config::qualified(basePath("separatorMargin"), 2u));
  return line;
}

class VisibleBuddy : public QWidget
{
public:
  VisibleBuddy() : _buddy(nullptr)
  {}
  void setVisible(bool visible) override
  {
    QWidget::setVisible(visible);
    if (_buddy)
      _buddy->setVisible(visible);
  }
  void setVisibleBuddy(QWidget *buddy)
  {
    _buddy = buddy;
  }
private:
  QWidget* _buddy;
};

class HorizontalButtonGroup : public VisibleBuddy
{
public:
  explicit HorizontalButtonGroup()
  {
    buttonLayout = new QHBoxLayout;
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setAlignment(Qt::AlignCenter | Qt::AlignTop);
    setLayout(buttonLayout);
  }

  template<typename... Widgets>
  static HorizontalButtonGroup* create(Widgets... widgets)
  {
    HorizontalButtonGroup* group = new HorizontalButtonGroup();
    auto unpack = {(group->addWidget(widgets), 0)...};
    return group;
  }

  void addWidget(QWidget *widget)
  {
    buttonLayout->addWidget(widget);
  }

private:
  QHBoxLayout* buttonLayout;
};

class NamedHorizontalButtonGroup : public VisibleBuddy
{
public:
  void setVisible();
  explicit NamedHorizontalButtonGroup(QString const& name)
  {
    buttonLayout = new QHBoxLayout;
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setAlignment(Qt::AlignCenter | Qt::AlignTop);

    QVBoxLayout *group = new QVBoxLayout;
    group->setContentsMargins(0, 0, 0, 0);
    group->setSpacing(0);
    group->addLayout(buttonLayout);
    group->addWidget(new GroupLabel(name));
    setLayout(group);
  }

  template<typename... Widgets>
  static NamedHorizontalButtonGroup* create(QString const& name, Widgets... widgets)
  {
    NamedHorizontalButtonGroup* group = new NamedHorizontalButtonGroup(name);
    auto unpack = {(group->addWidget(widgets), 0)...};
    return group;
  }

  void addWidget(QWidget *widget)
  {
    buttonLayout->insertWidget(buttonLayout->count(), widget);
  }

private:
  QHBoxLayout *buttonLayout;
};

class VerticalButtonGroup : public QWidget
{
public:
  explicit VerticalButtonGroup()
  {
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 2, 0, 0);
    layout->setSpacing(0);
    layout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setLayout(layout);
  }

  template<typename... Widgets>
  static VerticalButtonGroup* create(Widgets... widgets)
  {
    VerticalButtonGroup *group = new VerticalButtonGroup;
    auto unpack = {(group->addWidget(widgets), 0)...};
    return group;
  }

  void addWidget(QWidget *widget)
  {
    layout()->addWidget(widget);
  }
};

QWidget* addGroupWithSeparator(VisibleBuddy *group, std::string const& name, QHBoxLayout *layout, std::map<std::string, QWidget*> & mapping)
{
  QWidget *separator = makeGroupSeparator();
  group->setVisibleBuddy(separator);

  layout->addWidget(group);
  layout->addWidget(separator);

  mapping.emplace(name, group);

  return group;
}
} // unnamed namespace

// TODO: Need to fix alignment issues because big buttons in different groups are rather vertically centered instead being top-aligned
QWidget* MainToolbar::setupFileGroup()
{
  QToolButton *importButton = registerButton("fileImport", new BigToolButton(tr("Import"), basePath("fileImport")));

  NamedHorizontalButtonGroup *group = NamedHorizontalButtonGroup::create(tr("File"), importButton);
  return addGroupWithSeparator(group, "file", _layout, _groups);
}

QWidget* MainToolbar::setupViewGroup()
{
  QToolButton *allSeriesButton = registerButton("viewAllSeries", new InlineToolButton(tr("All series"), basePath("viewAllSeries")));
  QToolButton *numSeriesButton = registerButton("viewSingleSeries", new InlineToolButton(tr("Single series"), basePath("viewSingleSeries")));
  QToolButton *rowSeriesButton = registerButton("viewScrollable", new InlineToolButton(tr("Scrollable"), basePath("viewScrollable")));
  QToolButton *previewButton = registerButton("viewFullscreenPreview", new BigToolButton(tr("Fullscreen\npreview"), basePath("viewFullscreenPreview")));
  QToolButton *labButton = registerButton("viewLaboratory", new BigToolButton(tr("Enhance\nphotos"), basePath("viewLaboratory")));
  QToolButton *detailsButton = registerButton("viewPhotoDetails", new BigToolButton(tr("Photo\ndetails"), basePath("viewPhotoDetails")));
  QToolButton *previousSeries = registerButton("viewPreviousSeries", new InlineToolButton(basePath("viewPreviousSeries")));
  QToolButton *nextSeries = registerButton("viewNextSeries", new InlineToolButton(basePath("viewNextSeries")));

  NamedHorizontalButtonGroup *group = NamedHorizontalButtonGroup::create(tr("View"),
        VerticalButtonGroup::create(allSeriesButton, numSeriesButton, rowSeriesButton),
        previewButton,
        labButton,
        detailsButton,
        VerticalButtonGroup::create(previousSeries, nextSeries));

  return addGroupWithSeparator(group, "view", _layout, _groups);
}

QWidget* MainToolbar::setupSelectGroup()
{
  QToolButton *bestButton = registerButton("selectBest", new BigToolButton(tr("Best"), basePath("selectBest")));
  QToolButton *allButton = registerButton("selectAll", new InlineToolButton(tr("All"), basePath("selectAll")));
  QToolButton *invertButton = registerButton("selectInvert", new InlineToolButton(tr("Invert"), basePath("selectInvert")));
  QToolButton *clearButton = registerButton("selectClear", new InlineToolButton(tr("Clear"), basePath("selectClear")));

  NamedHorizontalButtonGroup *group = NamedHorizontalButtonGroup::create(tr("Select"),
        bestButton,
        VerticalButtonGroup::create(allButton, invertButton, clearButton));

  return addGroupWithSeparator(group, "select", _layout, _groups);
}

QWidget* MainToolbar::setupProcessGroup()
{
  QToolButton *deleteButton = registerButton("processDelete", new BigToolButton(tr("Delete"), basePath("processDelete")));
  QToolButton *moveButton = registerButton("processMove", new InlineToolButton(tr("Move"), basePath("processMove")));
  QToolButton *copyButton = registerButton("processCopy", new InlineToolButton(tr("Copy"), basePath("processCopy")));
  QToolButton *renameButton = registerButton("processRename", new InlineToolButton(tr("Rename"), basePath("processRename")));

  NamedHorizontalButtonGroup *group = NamedHorizontalButtonGroup::create(tr("Process"),
        deleteButton,
        VerticalButtonGroup::create(moveButton, copyButton, renameButton));

  return addGroupWithSeparator(group, "process", _layout, _groups);
}

QWidget* MainToolbar::setupHelpGroup()
{
  QToolButton *licenseButton = registerButton("helpLicense", new InlineToolButton(basePath("helpLicense")));

  HorizontalButtonGroup *group = HorizontalButtonGroup::create(licenseButton);

  return addGroupWithSeparator(group, "help", _layout, _groups);
}

void MainToolbar::setupHideButton(QBoxLayout *target)
{
  _hideButton = new InlineToolButton(basePath("hideNavigation"));
  _hideButton->setIconSize(config::qSize(basePath("hideButtonSize"), QSize(12, 12)));
  QObject::connect(_hideButton, &QToolButton::clicked, [this](){ setHidden(!_hidden); });
  target->addWidget(_hideButton, 0, Qt::AlignBottom);
}

} // namespace phobos
