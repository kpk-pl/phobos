#include <QHBoxLayout>
#include <QIcon>
#include <QSize>
#include "NavigationBar.h"
#include "ConfigExtension.h"
#include "ConfigPath.h"
#include "ImageProcessing/Utils/ColoredPixmap.h"

namespace phobos {

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

NavigationBar::NavigationBar(bool includeSlider) :
  leftButton(new IconButton(makeIcon("prevItemIcon"))),
  rightButton(new IconButton(makeIcon("nextItemIcon"))),
  prevSeriesButton(new IconButton(makeIcon("prevSeriesIcon"))),
  nextSeriesButton(new IconButton(makeIcon("nextSeriesIcon"))),
  allSeriesButton(new IconButton(makeIcon("allSeriesIcon"))),
  numSeriesButton(new IconButton(makeIcon("numSeriesIcon"))),
  oneSeriesButton(new IconButton(makeIcon("oneSeriesIcon"))),
  _slider(nullptr)
{
  leftButton->setToolTip(tr("Previous photo"));
  rightButton->setToolTip(tr("Next photo"));
  prevSeriesButton->setToolTip(tr("Previous series"));
  nextSeriesButton->setToolTip(tr("Next series"));
  allSeriesButton->setToolTip(tr("Return to main view with all series displayed"));
  numSeriesButton->setToolTip(tr("Switch to view with photos side by side"));
  oneSeriesButton->setToolTip(tr("Switch to view with whole series in one row"));

  QHBoxLayout* layout = new QHBoxLayout();
  layout->setSpacing(config::qualified(basePath("spacing"), 2u));
  setLayout(layout);

  layout->addWidget(allSeriesButton);
  layout->addWidget(numSeriesButton);
  layout->addWidget(oneSeriesButton);

  if (includeSlider)
  {
    _slider = new QSlider(Qt::Horizontal);
    _slider->setMaximum(100);
    _slider->setValue(100);
    _slider->setToolTip(tr("Zoom in / zoom out"));

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

  layout->addWidget(prevSeriesButton);
  layout->addWidget(leftButton);
  layout->addWidget(rightButton);
  layout->addWidget(nextSeriesButton);
}

void NavigationBar::setContentsMargins(int left, int top, int right, int bottom) const
{
  layout()->setContentsMargins(left, top, right, bottom);
}

} // namespace phobos
