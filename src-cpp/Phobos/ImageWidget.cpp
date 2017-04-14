#include <QMouseEvent>
#include <QPaintEvent>
#include <QPoint>
#include <QPainter>
#include "ImageWidget.h"
#include "ImageProcessing/ScalePixmap.h"

namespace phobos {

ImageWidget::ImageWidget(std::shared_ptr<QPixmap> const& pixmap)
{
    setScaledContents(false);
    setImagePixmap(pixmap);
}

void ImageWidget::setImagePixmap(std::shared_ptr<QPixmap> const& pixmap)
{
    _pixmap = pixmap;
    updateGeometry();
    update();
}

std::shared_ptr<QPixmap> const& ImageWidget::pixmap() const
{
    return _pixmap;
}

void ImageWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
        emit clicked();
}
void ImageWidget::paintEvent(QPaintEvent*)
{
    QPoint point;
    auto const& scaledPixmap = renderedPixmap();

    point.setX((width() - scaledPixmap->width()) / 2);
    point.setY((height() - scaledPixmap->height()) / 2);
    QPainter(this).drawPixmap(point, *scaledPixmap);
}

std::shared_ptr<QPixmap> ImageWidget::scaledPixmap(QSize const& size) const
{
    return std::make_shared<QPixmap>(iprocess::scalePixmap(*_pixmap, size));
}

std::shared_ptr<QPixmap> ImageWidget::renderedPixmap() const
{
    return scaledPixmap(size());
}

int ImageWidget::heightForWidth(const int width) const
{
    if (!_pixmap || !_pixmap->width())
        return 0;
    return _pixmap->height() * width / _pixmap->width();
}

int ImageWidget::widthForHeight(const int height) const
{
    if (!_pixmap || !_pixmap->height())
        return 0;
    return _pixmap->width() * height / _pixmap->height();
}

bool ImageWidget::hasHeightForWidth() const
{
    return static_cast<bool>(_pixmap);
}

bool ImageWidget::hasWidthForHeight() const
{
    return static_cast<bool>(_pixmap);
}

QSize ImageWidget::sizeHint() const
{
    if (_pixmap)
        return QSize();
    return _pixmap->size();
}

} // namespace phobos
