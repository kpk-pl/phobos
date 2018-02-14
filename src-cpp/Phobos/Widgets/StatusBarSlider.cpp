#include "Widgets/StatusBarSlider.h"
#include "Widgets/StatusBarButton.h"
#include "ConfigExtension.h"
#include <QSlider>
#include <QString>
#include <QHBoxLayout>
#include <QPushButton>
#include <QStyleOptionSlider>
#include <QStyle>
#include <QToolTip>

namespace phobos { namespace widgets {

namespace {
struct TooltipSlider : public QSlider
{
  explicit TooltipSlider(Qt::Orientation orientation) : QSlider(orientation)
  {}

  void sliderChange(QAbstractSlider::SliderChange change) override
  {
    QSlider::sliderChange(change);

    if (change == QAbstractSlider::SliderValueChange)
    {
      QStyleOptionSlider opt;
      initStyleOption(&opt);

      QRect const sr = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
      QToolTip::showText(mapToGlobal(sr.bottomLeft()), QString("%1%").arg(value()), this);
    }
  }
};
} // unnamed namespace

StatusBarSlider::StatusBarSlider(config::ConfigPath const& configPath) :
  slider(new TooltipSlider(Qt::Horizontal)),
  minus(new StatusBarButton(configPath("plusButton"))),
  plus(new StatusBarButton(configPath("minusButton")))
{
  QSize const sliderSize = config::qSize(configPath("sliderSize"), QSize(150, 15));

  slider->setFixedSize(sliderSize);
  slider->setStyleSheet(QString::fromStdString(config::qualified(configPath("sliderStyle"), std::string{})));

  QHBoxLayout *layout = new QHBoxLayout;
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(2);
  layout->addWidget(minus);
  layout->addWidget(slider);
  layout->addWidget(plus);

  setLayout(layout);
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  slider->setRange(0, 100);
  slider->setValue(100);
  slider->setPageStep(10);

  QObject::connect(minus, &QPushButton::clicked, [this](){ slider->setValue(slider->value() - slider->pageStep()); });
  QObject::connect(plus, &QPushButton::clicked, [this](){ slider->setValue(slider->value() + slider->pageStep()); });
  QObject::connect(slider, &QSlider::valueChanged, this, &StatusBarSlider::valueChanged);
}

}} // namespace phobos::widgets
