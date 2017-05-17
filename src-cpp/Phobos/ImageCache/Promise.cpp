#include <memory>
#include <QThreadPool>
#include "ImageCache/Promise.h"
#include "ImageProcessing/LoaderThread.h"

namespace phobos { namespace icache {

Promise::Promise(QImage const& readyImage) :
    readyImage(readyImage),
    callMetrics(false),
    threadPriority(0)
{
}

Promise::Promise(std::string const& imageFilename) :
    imageFilename(imageFilename),
    callMetrics(false),
    threadPriority(0)
{
}

PromisePtr Promise::create(QImage const& readyImage)
{
    return std::make_shared<Promise>(readyImage);
}

PromisePtr Promise::create(std::string const& filenameToLoad)
{
    return std::make_shared<Promise>(filenameToLoad);
}

void Promise::enableMetricsCall()
{
    callMetrics = true;
}

void Promise::setThreadPriority(int const priority)
{
    threadPriority = priority;
}

void Promise::start() const
{
    // TODO: pass size limit from config (only one max size is enough)
    // TODO: callMetrics as a setter in LoaderThread, refactor to one if-statement
    std::vector<QSize> vs = { QSize(1920, 1080) };
    auto loaderTask = std::make_unique<iprocess::LoaderThread>(
            *imageFilename, vs, callMetrics);

    loaderTask->setAutoDelete(true);

    if (callMetrics)
        QObject::connect(&loaderTask->readySignals, &iprocess::LoaderThreadSignals::metricsReady,
                this, &Promise::metricsReady, Qt::QueuedConnection);

    QObject::connect(&loaderTask->readySignals, &iprocess::LoaderThreadSignals::imageReady,
            this, &Promise::imageReadyFromThread, Qt::QueuedConnection);

    QThreadPool::globalInstance()->start(loaderTask.release(), threadPriority);
}

void Promise::imageReadyFromThread(QImage image)
{
    readyImage = image;
    emit imageReady(image);
}

}} // namespace phobos::icache
