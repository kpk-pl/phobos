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

void ImageWidget::setImage(QImage const& image)
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

void ImageWidget::setBorder(std::size_t const width)
{
  borderWidth = static_cast<int>(width);
  updateGeometry();
  update();
}

void ImageWidget::paintEvent(QPaintEvent*)
{
  QPainter painter(this);
  QSize const fullSize = size();

  painter.fillRect(QRect(QPoint(), fullSize), Qt::black);

  QImage const scaledImage = _image.scaled(fullSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  QPoint const point{(fullSize.width()  - scaledImage.width() ) / 2 + borderWidth,
                     (fullSize.height() - scaledImage.height()) / 2 + borderWidth};

  painter.drawImage(point, scaledImage);
}

int ImageWidget::heightForWidth(const int width) const
{
  if (_size.isNull() || !_size.width())
    return 0;

  if (width < 2*borderWidth)
    return 0;

  return _size.height() * (width - 2*borderWidth) / _size.width() + 2*borderWidth;
}

int ImageWidget::widthForHeight(const int height) const
{
  if (_size.isNull() || !_size.height())
    return 0;

  if (height < 2*borderWidth)
    return 0;

  return _size.width() * (height - 2*borderWidth) / _size.height() + 2*borderWidth;
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
