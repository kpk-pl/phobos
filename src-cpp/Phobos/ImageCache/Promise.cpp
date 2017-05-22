#include <easylogging++.h>
#include "ImageCache/Promise.h"
#include "Utils/Streaming.h"

namespace phobos { namespace icache {

Promise::Promise(std::unique_ptr<iprocess::LoaderThread> loaderThread,
                 FuturePtr const loaderFuture) :
    _loaderThread(std::move(loaderThread)),
    _future(loaderFuture),
    _loadedMetrics(false)
{
    LOG(INFO) << "P" << utils::stream::ObjId{}(this) << " : creating";

    QObject::connect(&_loaderThread->readySignals, &iprocess::LoaderThreadSignals::metricsReady,
                     this, &Promise::threadLoadedMetrics, Qt::QueuedConnection);
}


PromisePtr Promise::create(std::string const& filenameToLoad,
                           QImage const& initialPreloadImage)
{
    // TODO: pass size limit from config (only one max size is enough)
    std::vector<QSize> vs = { QSize(1920, 1080) };

    auto future = Future::create(initialPreloadImage);

    auto thread = std::make_unique<iprocess::LoaderThread>(filenameToLoad, vs);
    thread->setAutoDelete(false);
    thread->withMetrics(true);

    QObject::connect(&thread->readySignals, &iprocess::LoaderThreadSignals::imageReady,
                     future.get(), &Future::setImage, Qt::QueuedConnection);

    return std::make_shared<Promise>(std::move(thread), future);
}

void Promise::threadLoadedMetrics(phobos::iprocess::MetricPtr metrics)
{
    emit metricsReady(metrics);

    _loaderThread->withMetrics(false);
    QObject::disconnect(&_loaderThread->readySignals, &iprocess::LoaderThreadSignals::metricsReady, this, 0);
}

}} // namespace phobos::icache
