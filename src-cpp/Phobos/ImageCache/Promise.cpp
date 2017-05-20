#include <memory>
#include <easylogging++.h>
#include <QThreadPool>
#include "ImageCache/Promise.h"
#include "ImageProcessing/LoaderThread.h"
#include "Utils/Streaming.h"

namespace phobos { namespace icache {

Promise::Promise(QImage const& readyImage) :
    future(Future::create(readyImage))
{
}

PromisePtr Promise::create(QImage const& readyImage)
{
    return std::make_shared<Promise>(readyImage);
}

Promise::Promise()
{
}

PromisePtr Promise::create(std::string const& filenameToLoad, int const priority, bool callMetrics)
{
    // TODO: pass size limit from config (only one max size is enough)
    // TODO: callMetrics as a setter in LoaderThread, refactor to one if-statement
    std::vector<QSize> vs = { QSize(1920, 1080) };

    PromisePtr const promise = std::make_shared<Promise>();
    LOG(INFO) << "Creating promise (" << utils::stream::ObjId{}(promise)
              << ") for: " << filenameToLoad << " with priority " << priority
              << (callMetrics ? " with metrics" : " without metrics");

    auto loaderTask = std::make_unique<iprocess::LoaderThread>(
            filenameToLoad, vs, callMetrics);

    loaderTask->setAutoDelete(true);

    if (callMetrics)
        QObject::connect(&loaderTask->readySignals, &iprocess::LoaderThreadSignals::metricsReady,
                promise.get(), &Promise::threadLoadedMetrics, Qt::QueuedConnection);

    QObject::connect(&loaderTask->readySignals, &iprocess::LoaderThreadSignals::imageReady,
            promise.get(), &Promise::imageReadyFromThread, Qt::QueuedConnection);


    QThreadPool::globalInstance()->start(loaderTask.release(), priority);
    return promise;
}

void Promise::imageReadyFromThread(QImage image)
{
    LOG(INFO) << "Promise (" << utils::stream::ObjId{}(this) << ") loaded image";
    future->setImage(image);
    emit threadLoadedImage(image);
}

}} // namespace phobos::icache
