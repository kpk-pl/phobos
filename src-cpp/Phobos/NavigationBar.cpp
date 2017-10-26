#include <QHBoxLayout>
#include <QIcon>
#include <QSize>
#include "NavigationBar.h"
#include "ConfigExtension.h"
#include "ConfigPath.h"
#include "ImageProcessing/ColoredPixmap.h"

namespace phobos {

namespace {
  config::ConfigPath const basePath("navigationBar");

  QIcon makeIcon(std::string const& configName)
  {
    std::string const path = config::qualified(basePath(configName), std::string{});
    QColor const color = config::qColor(basePath("iconColor"), Qt::black);
    return iprocess::coloredPixmap(path, color, QSize(64, 64));
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

NavigationBar::NavigationBar(int capabilities) :
  _leftButton(nullptr), _rightButton(nullptr),
  _allSeries(nullptr), _numSeries(nullptr), _oneSeries(nullptr),
  _slider(nullptr)
{
  QHBoxLayout* layout = new QHBoxLayout();
  layout->setSpacing(config::qualified(basePath("spacing"), 2u));
  setLayout(layout);

  if (capabilities & Capability::ALL_SERIES)
  {
    _allSeries = new IconButton(makeIcon("allSeriesIcon"));
    layout->addWidget(_allSeries);
  }

  if (capabilities & Capability::NUM_SERIES)
  {
    _numSeries = new IconButton(makeIcon("numSeriesIcon"));
    layout->addWidget(_numSeries);
  }

  if (capabilities & Capability::ONE_SERIES)
  {
    _oneSeries = new IconButton(makeIcon("oneSeriesIcon"));
    layout->addWidget(_oneSeries);
  }

  if (capabilities & Capability::SLIDER)
  {
    _slider = new QSlider(Qt::Horizontal);
    _slider->setMaximum(100);
    _slider->setValue(100);

    QWidget* lSpacing = new QWidget();
    lSpacing->setMinimumWidth(15);
    QWidget* rSpacing = new QWidget();
    rSpacing->setMinimumWidth(15);

    layout->addWidget(lSpacing);
    layout->addWidget(_slider);
    layout->addWidget(rSpacing);
  }
  else
  {
    layout->addStretch();
  }

  if (capabilities & Capability::LEFT)
  {
    _leftButton = new IconButton(QIcon::fromTheme("go-previous", QIcon(":icons/go-previous.png")));
    layout->addWidget(_leftButton);
  }

  if (capabilities & Capability::RIGHT)
  {
    _rightButton = new IconButton(QIcon::fromTheme("go-next", QIcon(":icons/go-next.png")));
    layout->addWidget(_rightButton);
  }
}

void NavigationBar::setContentsMargins(int left, int top, int right, int bottom) const
{
  layout()->setContentsMargins(left, top, right, bottom);
}

} // namespace phobos
