#include <easylogging++.h>
#include "PhotoContainers/Series.h"
#include "ImageCache/Cache.h"
#include "ImageCache/Promise.h"
#include "ImageCache/Future.h"
#include "Utils/Algorithm.h"
#include "Utils/Preload.h"

namespace phobos { namespace icache {

Cache::Cache(pcontainer::Set const& photoSet) :
    photoSet(photoSet)
{
}

FuturePtrVec Cache::getSeries(QUuid const seriesUuid) const
{
    pcontainer::SeriesPtr const& requestedSeries = photoSet.findSeries(seriesUuid);
    assert(requestedSeries);

    FuturePtrVec result;
    result.reserve(requestedSeries->size());

    for (auto const& photo : *requestedSeries)
        result.push_back(getFuture(photo->fileName()));

    return result;
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

FuturePtr Cache::getFuture(std::string const& imageFilename) const
{
    // If image already in cache
    auto const imageIt = imageCache.find(imageFilename);
    if (imageIt != imageCache.end())
        return Future::createReady(imageIt->second);

    // if not, maybe it is already being loaded
    auto const promiseIt = promiseMap.find(imageFilename);
    if (promiseIt != promiseMap.end())
        return promiseIt->second->future();

    // if not, load it in thread
    return makeLoadingPromiseAndFuture(imageFilename);
}

QImage Cache::getPreloadNow(std::string const& imageFilename) const
{
    auto const preloadIt = preloadCache.find(imageFilename);
    if (preloadIt != preloadCache.end())
        return preloadIt->second;

    // TODO: this function will accept preferred size
    // return preload of this size
    // cache same sizes to not produce copies each call
    static QImage const preloadImage = utils::preloadImage(QSize(400, 300));
    return preloadImage;
}

// TODO: a lot of methods in Cache take imageFilename
// maybe create a special tyle of CacheTransaction (?) that will hold this and be passed to all
// methods in here?

FuturePtr Cache::makeLoadingPromiseAndFuture(std::string const& imageFilename) const
{
    bool const loadMetrics = !hasMetrics(imageFilename);
    auto newPromise = Promise::create(imageFilename, getPreloadNow(imageFilename), loadMetrics);

    if (loadMetrics)
    {
        QObject::connect(newPromise.get(), &Promise::threadLoadedMetrics,
                [this, imageFilename](iprocess::MetricPtr metrics){
                       updateMetrics(imageFilename, metrics);
                });
    }

    QObject::connect(newPromise->future().get(), &Future::imageReady,
            [this, imageFilename](QImage image){
                   updateImage(imageFilename, image);
            });

    promiseMap.emplace(imageFilename, newPromise);
    return newPromise->future();
}

void Cache::updateMetrics(std::string const& imageFilename, iprocess::MetricPtr const& metrics) const
{
    metricCache.emplace(imageFilename, *metrics);

    // TODO: keep metrics object, not shared_ptrs
    // TODO: scored metrics fill
}

void Cache::updateImage(std::string const& imageFilename, QImage const& image) const
{
    preloadCache.emplace(imageFilename, image);
    LOG(INFO) << "Updated image cache for: " << imageFilename;

    auto promiseIt = promiseMap.find(imageFilename);
    if (promiseIt != promiseMap.end())
        promiseMap.erase(promiseIt);
}

}} // namespace phobos::icache
