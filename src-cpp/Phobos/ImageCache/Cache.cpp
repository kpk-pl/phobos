#include <easylogging++.h>
#include <QThreadPool>
#include "PhotoContainers/Series.h"
#include "ImageCache/Cache.h"
#include "Utils/Algorithm.h"
#include "Utils/Preload.h"
#include "Utils/Asserted.h"
#include "ConfigExtension.h"
#include "ImageProcessing/MetricsAggregate.h"
#include "ImageProcessing/MetricsIO.h"

namespace phobos { namespace icache {

Cache::Cache(pcontainer::Set const& photoSet) :
    photoSet(photoSet)
{
}

namespace {
    QImage getInitialPreload()
    {
        static QImage const preloadImage =
            utils::preloadImage(config::qSize("imageCache.preloadSize", QSize(320, 240)));

        return preloadImage;
    }
} // unnamed namespace

QImage Cache::getImage(pcontainer::Item const& item) const
{
    auto it = imageCache.find(item.fileName());
    if (it != imageCache.end() && !it->second.full.isNull())
        return it->second.full;

    startThreadForItem(item);

    if (!it->second.preload.isNull())
        return it->second.preload;
    else
        return getInitialPreload();
}

QImage Cache::getPreload(pcontainer::Item const& item) const
{
    auto it = imageCache.find(item.fileName());
    if (it != imageCache.end() && !it->second.preload.isNull())
        return it->second.preload;

    startThreadForItem(item);
    return getInitialPreload();
}

std::unique_ptr<iprocess::LoaderThread> Cache::makeLoadingThread(std::string const& filename) const
{
    // TODO: pass size limit from config (only one max size is enough)
    std::vector<QSize> vs = { config::qSize("imageCache.fullSize", QSize(1920, 1080)) };

    auto thread = std::make_unique<iprocess::LoaderThread>(filename, vs);

    thread->setAutoDelete(true);

    if (!hasMetrics(filename))
    {
        thread->withMetrics(hasMetrics(filename));
        QObject::connect(&thread->readySignals, &iprocess::LoaderThreadSignals::metricsReady,
                         this, &Cache::metricsReadyFromThread, Qt::QueuedConnection);
    }

    QObject::connect(&thread->readySignals, &iprocess::LoaderThreadSignals::imageReady,
                     this, &Cache::imageReadyFromThread, Qt::QueuedConnection);

    return thread;
}

void Cache::startThreadForItem(pcontainer::Item const& item) const
{
    if (utils::valueIn(item.fileName(), loadingImageSeriesId))
        return;

    loadingImageSeriesId.emplace(item.fileName(), item.seriesUuid());

    auto thread = makeLoadingThread(item.fileName());
    QThreadPool::globalInstance()->start(thread.release());
}

void Cache::imageReadyFromThread(QImage image, std::string fileName)
{
    auto& entry = imageCache[fileName];
    entry.full = image;

    if (entry.preload.isNull())
    {
        auto const preloadSize = config::qSize("imageCache.preloadSize", QSize(320, 240));
        entry.preload = image.scaled(preloadSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    auto const uuidIt = loadingImageSeriesId.find(fileName);
    assert(uuidIt != loadingImageSeriesId.end());

    QUuid const seriesId = uuidIt->second;
    loadingImageSeriesId.erase(uuidIt);

    emit updateImage(seriesId, fileName, image);
}

void Cache::metricsReadyFromThread(iprocess::MetricPtr metrics, std::string fileName)
{
    metricCache.emplace(fileName, *metrics);

    auto const& seriesUuid = utils::asserted::fromMap(loadingImageSeriesId, fileName);
    auto const& series = photoSet.findSeries(seriesUuid);

    if (!std::any_of(series->begin(), series->end(),
                [this](pcontainer::ItemPtr const& item){
                    return utils::valueIn(item->fileName(), metricCache);
                }))
    {
        return;
    }

    std::vector<iprocess::Metric const*> allMetrics;
    allMetrics.reserve(series->size());
    for (auto const& item : *series)
        allMetrics.push_back(&metricCache[item->fileName()]);

    std::vector<iprocess::ScoredMetric> scoredMetrics = iprocess::aggregateMetrics(allMetrics);
    assert(scoredMetrics.size() == series->size());

    for (std::size_t i = 0; i < series->size(); ++i)
    {
        auto const& fileName = series->item(i)->fileName();
        scoredMetricCache.emplace(fileName, std::move(scoredMetrics[i]));

        LOG_IF(config::qualified("logging.metrics", false), DEBUG)
           << "Calculated series metrics" << std::endl
           << "photoItem: " << fileName << std::endl
           << "metric: " << metricCache[fileName] << std::endl
           << "scoredMetric: " << scoredMetricCache[fileName];
    }
}

bool Cache::hasMetrics(std::string const& photoFilename) const
{
    return utils::valueIn(photoFilename, metricCache);
}

bool Cache::hasScoredMetrics(std::string const& photoFilename) const
{
    return utils::valueIn(photoFilename, scoredMetricCache);
}

iprocess::Metric const& Cache::getMetrics(std::string const& photoFilename) const
{
    auto const it = metricCache.find(photoFilename);
    if (it == metricCache.end())
        throw NoMetricException(photoFilename);

    return it->second;
}

iprocess::ScoredMetric const& Cache::getScoredMetrics(std::string const& photoFilename) const
{
    auto const it = scoredMetricCache.find(photoFilename);
    if (it == scoredMetricCache.end())
        throw NoMetricException(photoFilename);

    return it->second;
}

}} // namespace phobos::icache
