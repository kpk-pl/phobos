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

FuturePtr Cache::getFuture(std::string const& imageFilename) const
{
    if (!utils::valueIn(imageFilename, imagePromiseMap))
        makeNewPromise(imageFilename);

    auto const promiseIt = imagePromiseMap.find(imageFilename);
    assert(promiseIt != imagePromiseMap.end());
    return promiseIt->second->future();
}

QImage Cache::getInitialPreload() const
{
    // TODO: this function will accept preferred size
    // return preload of this size
    // cache same sizes to not produce copies each call
    static QImage const preloadImage = utils::preloadImage(QSize(400, 300));
    return preloadImage;
}

// TODO: a lot of methods in Cache take imageFilename
// maybe create a special tyle of CacheTransaction (?) that will hold this and be passed to all
// methods in here?

void Cache::makeNewPromise(std::string const& imageFilename) const
{
    auto newPromise = Promise::create(imageFilename, getInitialPreload());

    QObject::connect(newPromise.get(), &Promise::threadLoadedMetrics,
            [this, imageFilename](iprocess::MetricPtr metrics){
                   updateMetrics(imageFilename, metrics);
            });

    imagePromiseMap.emplace(imageFilename, std::move(newPromise));
}

void Cache::updateMetrics(std::string const& imageFilename, iprocess::MetricPtr const& metrics) const
{
    metricCache.emplace(imageFilename, *metrics);

    // TODO: keep metrics object, not shared_ptrs
    // TODO: scored metrics fill
    //
    // When scored metrics done, disconnect signals
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
