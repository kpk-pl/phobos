#include <easylogging++.h>
#include "ImageCache/Promise.h"
#include "Utils/Streaming.h"

namespace phobos { namespace icache {

PromisePtr Promise::create(QImage const& readyImage)
{
    auto const promise = std::make_shared<Promise>();
    LOG(DEBUG) << "P" << utils::stream::ObjId{}(promise) << " : creating with ready image";

    promise->_future = Future::createReady(readyImage);
    return promise;
}

PromisePtr Promise::create(std::string const& filenameToLoad,
                           QImage const& preloadImage,
                           bool callMetrics)
{
    // TODO: pass size limit from config (only one max size is enough)
    std::vector<QSize> vs = { QSize(1920, 1080) };

    PromisePtr const promise = std::make_shared<Promise>();
    LOG(INFO) << "P" << utils::stream::ObjId{}(promise)
              << " : " << filenameToLoad << (callMetrics ? " with metrics" : " without metrics");

    promise->_future = Future::createPreload(preloadImage);

    promise->_loadingThread = std::make_unique<iprocess::LoaderThread>(filenameToLoad, vs);
    auto& loadingThread = *promise->_loadingThread;
    loadingThread.setAutoDelete(true);

    if (callMetrics)
    {
        loadingThread.withMetrics(true);
        QObject::connect(&loadingThread.readySignals, &iprocess::LoaderThreadSignals::metricsReady,
                         promise.get(), &Promise::threadLoadedMetrics, Qt::QueuedConnection);
    }

    QObject::connect(&loadingThread.readySignals, &iprocess::LoaderThreadSignals::imageReady,
                     promise->_future.get(), &Future::setImage, Qt::QueuedConnection);

    return promise;
}

}} // namespace phobos::icache
