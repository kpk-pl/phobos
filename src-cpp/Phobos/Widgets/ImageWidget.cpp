#include <QPaintEvent>
#include <QPoint>
#include <QPainter>
#include "Widgets/ImageWidget.h"
#include "ImageProcessing/ScalePixmap.h"

namespace phobos { namespace widgets {

ImageWidget::ImageWidget() :
  soul(this)
{
  setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
  setScaledContents(false);
}

ImageWidget::ImageWidget(QImage const& image) :
  ImageWidget()
{
  setImage(image);
}

void ImageWidget::setImage(QImage image)
{
  _image = image;
  updateGeometry();
  update();
}

void ImageWidget::setMetrics(iprocess::MetricPtr metrics)
{
  if (!metrics)
    return;

  _metrics = metrics;
  update();
}

void ImageWidget::paintEvent(QPaintEvent*)
{
    QPoint point;
    QImage const scaledImage = _image.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    point.setX((width() - scaledImage.width()) / 2);
    point.setY((height() - scaledImage.height()) / 2);
    QPainter(this).drawImage(point, scaledImage);
}

int ImageWidget::heightForWidth(const int width) const
{
    if (_image.isNull() && !_image.width())
        return 0;
    return _image.height() * width / _image.width();
}

int ImageWidget::widthForHeight(const int height) const
{
    if (_image.isNull() || !_image.height())
        return 0;
    return _image.width() * height / _image.height();
}

bool ImageWidget::hasHeightForWidth() const
{
    return !_image.isNull();
}

bool ImageWidget::hasWidthForHeight() const
{
    return !_image.isNull();
}

}} // namespace phobos::widgets
