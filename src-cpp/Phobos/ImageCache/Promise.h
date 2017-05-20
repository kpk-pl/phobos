#ifndef PHOBOS_IMAGECACHE_PROMISE_H_
#define PHOBOS_IMAGECACHE_PROMISE_H_

#include <QObject>
#include <QImage>
#include "ImageCache/PromiseFwd.h"
#include "ImageCache/Future.h"
#include "ImageProcessing/Metrics.h"

namespace phobos { namespace icache {

class Promise : public QObject
{
    Q_OBJECT

public:
    explicit Promise(QImage const& readyImage);
    explicit Promise();

    static PromisePtr create(QImage const& readyImage);
    static PromisePtr create(std::string const& filenameToLoad, int const priority, bool callMetrics);

    FuturePtr future;

signals:
    void threadLoadedMetrics(phobos::iprocess::MetricPtr);
    void threadLoadedImage(QImage);

private slots:
    void imageReadyFromThread(QImage image);
};

}} // namespace phobos::icache

#endif // PHOBOS_IMAGECACHE_PROMISE_H_
