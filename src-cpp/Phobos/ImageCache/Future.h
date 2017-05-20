#ifndef IMAGE_CACHE_FUTURE_H_
#define IMAGE_CACHE_FUTURE_H_

#include <QObject>
#include <QImage>
#include "ImageCache/FutureFwd.h"

namespace phobos { namespace icache {

class Future : public QObject
{
    Q_OBJECT

friend class Promise;

public:
    explicit Future();
    explicit Future(QImage const& readyImage);

    static FuturePtr create();
    static FuturePtr create(QImage const& readyImage);

    operator bool() const { return !readyImage.isNull(); }
    QImage getImage() const { return readyImage; }

signals:
    void imageReady(QImage);

private:
    void setImage(QImage const& image);

    QImage readyImage;
};

}} // namespace phobos::icache

#endif // IMAGE_CACHE_FUTURE_H_
