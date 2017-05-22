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
    Future(QImage const& initialPreloadImage);
    static FuturePtr create(QImage const& initialPreloadImage);

    QImage getImage() const { return readyImage; }
    void voidImage();

    QImage getPreloadImage() const { return preloadImage; }

signals:
    void changed();

private slots:
    void setImage(QImage image);

private:
    QImage preloadImage;
    QImage readyImage;
    bool preloadReady;
};

}} // namespace phobos::icache

#endif // IMAGE_CACHE_FUTURE_H_
