#include "ImageCache/Cache.h"
#include "ImageCache/Promise.h"

namespace phobos { namespace icache {

Cache::Cache(pcontainer::Set const& photoSet) :
    photoSet(photoSet)
{

}

ConstPromisePtrVec Cache::getSeries(QUuid const seriesUuid) const
{
    pcontainer::SeriesPtr const& requestedSeries = photoSet.findSeries(seriesUuid);
    assert(requestedSeries);

    ConstPromisePtrVec result;
    result.reserve(requestedSeries->size());

    for (auto const& photo : *requestedSeries)
        result.push_back(makePromise(photo->fileName()));

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

ConstPromisePtr Cache::makePromise(std::string const& imageFilename) const
{
    auto const imageIt = preloadCache.find(imageFilename);
    if (imageIt != preloadCache.end())
        return Promise::create(imageIt->second);

    auto const promiseIt = promiseMap.find(imageFilename);
    if (promiseIt != promiseMap.end())
        return promiseIt->second;

    return makeLoadingPromise(imageFilename);
}

ConstPromisePtr Cache::makeLoadingPromise(std::string const& imageFilename) const
{
    auto newPromise = Promise::create(imageFilename);

    if (!hasMetrics(imageFilename))
    {
        newPromise->enableMetricsCall();
        QObject::connect(newPromise.get(), &Promise::metricsReady,
                [this, imageFilename](iprocess::MetricPtr metrics){
                       updateMetrics(imageFilename, metrics);
                });
    }

    QObject::connect(newPromise.get(), &Promise::imageReady,
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

}

}} // namespace phobos::icache
