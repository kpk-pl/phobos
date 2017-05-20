#include <easylogging++.h>
#include "ImageCache/Cache.h"
#include "ImageCache/Promise.h"
#include "ImageCache/Future.h"

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
    return metricCache.find(photoFilename) != metricCache.end();
}

bool Cache::hasScoredMetrics(std::string const& photoFilename) const
{
    return scoredMetricCache.find(photoFilename) != scoredMetricCache.end();
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
    auto const imageIt = preloadCache.find(imageFilename);
    if (imageIt != preloadCache.end())
        return Future::create(imageIt->second);

    auto const promiseIt = promiseMap.find(imageFilename);
    if (promiseIt != promiseMap.end())
        return promiseIt->second->future;

    return makeLoadingPromise(imageFilename)->future;
}

ConstPromisePtr Cache::makeLoadingPromise(std::string const& imageFilename) const
{
    bool const loadMetrics = !hasMetrics(imageFilename);
    auto newPromise = Promise::create(imageFilename, 1, loadMetrics);

    if (loadMetrics)
    {
        QObject::connect(newPromise.get(), &Promise::threadLoadedMetrics,
                [this, imageFilename](iprocess::MetricPtr metrics){
                       updateMetrics(imageFilename, metrics);
                });
    }

    QObject::connect(newPromise.get(), &Promise::threadLoadedImage,
            [this, imageFilename](QImage image){
                   updateImage(imageFilename, image);
            });

    promiseMap.emplace(imageFilename, newPromise);
    return newPromise;
}

void Cache::updateMetrics(std::string const& imageFilename, iprocess::MetricPtr const& metrics) const
{
    // TODO:
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
