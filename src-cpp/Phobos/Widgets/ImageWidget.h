#ifndef WIDGETS_IMAGEWIDGET_H
#define WIDGETS_IMAGEWIDGET_H

#include <memory>
#include <QImage>
#include <QLabel>
#include <QSize>
#include "Widgets/HeightResizeableInterface.h"
#include "ImageProcessing/MetricsFwd.h"

namespace phobos { namespace widgets {

class ImageWidget : public QLabel, public HeightResizeableInterface
{
    Q_OBJECT

public:
    explicit ImageWidget(QImage const& image);

    QImage image() const { return _image; }
    iprocess::MetricPtr metrics() const { return _metrics; }

    int heightForWidth(const int width) const override;
    int widthForHeight(const int height) const override;

    bool hasHeightForWidth() const override;
    bool hasWidthForHeight() const override;

    void mousePressEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

public slots:
    void setImage(QImage image);
    void setMetrics(iprocess::MetricPtr metrics);

signals:
    void clicked();

private:
    QImage _image;
    iprocess::MetricPtr _metrics;
};

}} // namespace phobos::widgets

#endif // WIDGETS_IMAGEWIDGET_H