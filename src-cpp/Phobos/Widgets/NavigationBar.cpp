#include "ConfigExtension.h"
#include "ConfigPath.h"
#include "Widgets/NavigationBar.h"
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

  QIcon makeIcon(std::string const& configName)
  {
    std::string const path = config::qualified(basePath(configName), std::string{});
    QColor const color = config::qColor(basePath("iconColor"), Qt::black);
    return iprocess::utils::coloredPixmap(path, color, QSize(64, 64));
  }

  class IconButton : public QPushButton
  {
  public:
    IconButton(QIcon const& icon, QWidget* parent = nullptr) :
        QPushButton(icon, "", parent),
        margin(config::qualified(basePath("buttonMargin"), 5u))
    {
      setIconSize(config::qSize(basePath("buttonSize"), QSize(40, 40)));
      setContentsMargins(margin, margin, margin, margin);
    }

    QSize sizeHint() const override
    {
      QSize iSize = iconSize();
      return QSize(iSize.width() + 2*margin, iSize.height() + 2*margin);
    }
  private:
    std::size_t const margin;
  };
} // unnamed namespace

NavigationBar::NavigationBar() :
  _slider(nullptr)
{
  _layout = new QHBoxLayout();
  _layout->setSpacing(config::qualified(basePath("spacing"), 2u));
  setLayout(_layout);
}

void NavigationBar::setContentsMargins(int left, int top, int right, int bottom) const
{
  layout()->setContentsMargins(left, top, right, bottom);
}

QPushButton* NavigationBar::button(std::string const& name) const
{
  return utils::asserted::fromMap(_buttons, name);
}

QPushButton* NavigationBar::addButton(std::string const& name, std::string const& icon)
{
  QPushButton* &button = _buttons[name];

  if (!button)
  {
    button = new IconButton(makeIcon(icon));
    _layout->addWidget(button);
  }

  return button;
}

QSlider* NavigationBar::addSlider()
{
  _slider = new QSlider(Qt::Horizontal);
  _slider->setMaximum(100);
  _slider->setValue(100);

  QWidget* lSpacing = new QWidget();
  lSpacing->setMinimumWidth(15);
  QWidget* rSpacing = new QWidget();
  rSpacing->setMinimumWidth(15);

  _layout->addWidget(lSpacing);
  _layout->addWidget(_slider);
  _layout->addWidget(rSpacing);

  return _slider;
}

void NavigationBar::addStretch()
{
  _layout->addStretch();
}

void NavigationBar::addSeparator()
{

}

}} // namespace phobos::widgets
