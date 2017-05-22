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
    explicit Promise() = default;

    static PromisePtr create(QImage const& readyImage);

    static PromisePtr create(std::string const& filenameToLoad,
                             QImage const& preloadImage,
                             bool callMetrics);

    FuturePtr future() const { return _future; }

signals:
    void threadLoadedMetrics(phobos::iprocess::MetricPtr);

private:
    FuturePtr _future;
    std::unique_ptr<iprocess::LoaderThread> _loadingThread;
};

}} // namespace phobos::icache

#endif // PHOBOS_IMAGECACHE_PROMISE_H_
