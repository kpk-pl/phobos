#ifndef PHOBOS_IMAGECACHE_PROMISE_H_
#define PHOBOS_IMAGECACHE_PROMISE_H_

#include <memory>
#include <QObject>
#include <QImage>
#include "ImageCache/PromiseFwd.h"
#include "ImageCache/Future.h"
#include "ImageProcessing/Metrics.h"
#include "ImageProcessing/LoaderThread.h"

namespace phobos { namespace icache {

class Promise : public QObject
{
    Q_OBJECT

friend class Cache;

public:
    explicit Promise(std::unique_ptr<iprocess::LoaderThread> loaderThread,
                     FuturePtr const loaderFuture);

    static PromisePtr create(std::string const& filenameToLoad,
                             QImage const& initialPreloadImage);

    FuturePtr future() const { return _future; }

signals:
    void metricsReady(phobos::iprocess::MetricPtr);

private slots:
    void threadLoadedMetrics(phobos::iprocess::MetricPtr);

private:
    std::unique_ptr<iprocess::LoaderThread> _loaderThread;
    FuturePtr const _future;
    bool _loadedMetrics;
};

}} // namespace phobos::icache

#endif // PHOBOS_IMAGECACHE_PROMISE_H_
