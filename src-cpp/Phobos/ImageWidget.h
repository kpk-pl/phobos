#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <memory>
#include <QPixmap>
#include <QLabel>
#include <QSize>
#include "HeightResizeableInterface.h"

namespace phobos {

class ImageWidget : public QLabel, public HeightResizeableInterface
{
    Q_OBJECT

public:
    explicit ImageWidget(std::shared_ptr<QPixmap> const& pixmap);

    std::shared_ptr<QPixmap> const& pixmap() const;
    std::shared_ptr<QPixmap> scaledPixmap(QSize const& size) const;

    int heightForWidth(const int width) const override;
    int widthForHeight(const int height) const override;

    bool hasHeightForWidth() const override;
    bool hasWidthForHeight() const override;

    void mousePressEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

public slots:
    void setImagePixmap(std::shared_ptr<QPixmap> const& pixmap);

signals:
    void clicked();

protected:
    virtual std::shared_ptr<QPixmap> renderedPixmap() const;

private:
    std::shared_ptr<QPixmap> _pixmap;
};

} // namespace phobos

#endif // IMAGEWIDGET_H
