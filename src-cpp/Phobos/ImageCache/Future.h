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
    enum class CreateMode { Ready, Preload };

    explicit Future(CreateMode mode, QImage const& image);

    static FuturePtr createReady(QImage const& readyImage);
    static FuturePtr createPreload(QImage const& preloadImage);

    operator bool() const { return !readyImage.isNull(); }
    QImage getImage() const { return readyImage; }
    QImage getPreload() const { return preloadImage; }

signals:
    void imageReady(QImage);

private slots:
    void setImage(QImage image);

private:
    QImage preloadImage;
    QImage readyImage;
};

}} // namespace phobos::icache

#endif // IMAGE_CACHE_FUTURE_H_
