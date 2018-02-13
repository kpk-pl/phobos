#include "ConfigExtension.h"
#include "ConfigPath.h"
#include "Widgets/NavigationBar.h"
#include "Widgets/HVLine.h"
#include "ImageProcessing/Utils/ColoredPixmap.h"
#include "Utils/Asserted.h"
#include <QHBoxLayout>
#include <QIcon>
#include <QSize>
#include <QPushButton>
#include <QSlider>

namespace phobos { namespace widgets {

namespace {
config::ConfigPath const basePath("navigationBar");

class IconButton : public QPushButton
{
public:
  IconButton(config::ConfigPath const& path, QSize const& size = QSize(), QWidget* parent = nullptr) :
    QPushButton(iprocess::utils::coloredPixmap(path, QSize(64, 64)), "", parent),
    margin(config::qualified(path("margin"), config::qualified(basePath("buttonMargin"), 5u)))
  {
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    auto const btnSize = size.isEmpty() ? config::qSize(basePath("buttonSize"), QSize(40, 40)) : size;
    setIconSize(btnSize - 2*QSize(margin, margin));
    setContentsMargins(margin, margin, margin, margin);
  }

  QSize sizeHint() const override
  {
    return QSize(iconSize() + 2*QSize(margin, margin));
  }
private:
  std::size_t const margin;
};
} // unnamed namespace

NavigationBar::NavigationBar(QWidget *parent) :
  QWidget(parent), hidden(false)
{
  _layout = new QHBoxLayout;
  _layout->setContentsMargins(0, 0, 0, 0);
  _layout->setSpacing(config::qualified(basePath("spacing"), 2u));

  _showHideButton = new IconButton(basePath("hideNavigation"), QSize(15, 15));

  QHBoxLayout *mainLayout = new QHBoxLayout;
  mainLayout->addLayout(_layout);
  mainLayout->addWidget(_showHideButton, 0, Qt::AlignTop | Qt::AlignRight);
  QObject::connect(_showHideButton, &QPushButton::clicked, this, &NavigationBar::showHideAction);

  setLayout(mainLayout);
}

void NavigationBar::setContentsMargins(int left, int top, int right, int bottom) const
{
  layout()->setContentsMargins(left, top, right, bottom);
}

QPushButton* NavigationBar::button(std::string const& name) const
{
  return utils::asserted::fromMap(_buttons, name);
}

QPushButton* NavigationBar::addButton(std::string const& name)
{
  QPushButton* &button = _buttons[name];

  if (!button)
  {
    button = new IconButton(basePath(name));
    _layout->addWidget(button);
  }

  return button;
}

void NavigationBar::addStretch()
{
  _layout->addStretch();
}

void NavigationBar::addSeparator()
{
  HVLine *line = new HVLine(Qt::Vertical);
  line->setMinimumWidth(10);
  _layout->addWidget(line);
}

void NavigationBar::showHideAction()
{
  hidden ? showButtons() : hideButtons();
}

void NavigationBar::showButtons()
{
  for (int i = 0; i < _layout->count(); ++i)
    if (QWidget* wgt = _layout->itemAt(i)->widget())
      wgt->show();

  _showHideButton->setIcon(iprocess::utils::coloredPixmap(basePath("hideNavigation"), QSize(64, 64)));

  hidden = false;
}

void NavigationBar::hideButtons()
{
  for (int i = 0; i < _layout->count(); ++i)
    if (QWidget* wgt = _layout->itemAt(i)->widget())
      wgt->hide();

  _showHideButton->setIcon(iprocess::utils::coloredPixmap(basePath("showNavigation"), QSize(64, 64)));

  hidden = true;
}

}} // namespace phobos::widgets
