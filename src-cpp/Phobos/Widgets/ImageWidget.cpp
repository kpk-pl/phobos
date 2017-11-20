#include <QPaintEvent>
#include <QPoint>
#include <QPainter>
#include "Widgets/ImageWidget.h"
#include "ImageProcessing/ScalePixmap.h"

namespace phobos { namespace widgets {

ImageWidget::ImageWidget(QSize const suggestedSize) :
  _size(suggestedSize), soul(this)
{
  setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
  setScaledContents(false);
}

ImageWidget::ImageWidget(QImage const& image) :
  ImageWidget(image.size())
{
  setImage(image);
}

void ImageWidget::setImage(QImage image)
{
  _image = image;
  _size = image.size();
  updateGeometry();
  update();
}

void ImageWidget::setMetrics(iprocess::metric::MetricPtr metrics)
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
  if (_size.isNull() || !_size.width())
    return 0;
  return _size.height() * width / _size.width();
}

int ImageWidget::widthForHeight(const int height) const
{
  if (_size.isNull() || !_size.height())
    return 0;
  return _size.width() * height / _size.height();
}

bool ImageWidget::hasHeightForWidth() const
{
  return _size.isValid();
}

bool ImageWidget::hasWidthForHeight() const
{
  return _size.isValid();
}

}} // namespace phobos::widgets
