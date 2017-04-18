#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <memory>
#include <QImage>
#include <QLabel>
#include <QSize>
#include "HeightResizeableInterface.h"

namespace phobos {

class ImageWidget : public QLabel, public HeightResizeableInterface
{
    Q_OBJECT

public:
    explicit ImageWidget(QImage const& image);

    QImage image() const { return _image; }

    int heightForWidth(const int width) const override;
    int widthForHeight(const int height) const override;

    bool hasHeightForWidth() const override;
    bool hasWidthForHeight() const override;

    void mousePressEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

public slots:
    void setImage(QImage image);

signals:
    void clicked();

private:
    QImage _image;
};

} // namespace phobos

#endif // IMAGEWIDGET_H
