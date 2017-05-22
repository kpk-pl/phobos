#ifndef PHOBOS_IMAGECACHE_CACHE_H_
#define PHOBOS_IMAGECACHE_CACHE_H_

#include <unordered_map>
#include <stdexcept>
#include <QObject>
#include <QImage>
#include "ImageCache/CacheFwd.h"
#include "ImageCache/PromiseFwd.h"
#include "ImageCache/FutureFwd.h"
#include "PhotoContainers/Set.h"

namespace phobos { namespace icache {

class Cache : public QObject
{
    Q_OBJECT

    class NoMetricException : public std::runtime_error
    {
    public:
        NoMetricException(std::string const& filename) :
            std::runtime_error("No metrics available for " + filename)
        {}
    };

public:
    explicit Cache(pcontainer::Set const& photoSet);

    FuturePtrVec getSeries(QUuid const seriesUuid) const;

    bool hasMetrics(std::string const& photoFilename) const;
    bool hasScoredMetrics(std::string const& photoFilename) const;
    iprocess::Metric const& getMetrics(std::string const& photoFilename) const;
    iprocess::ScoredMetric const& getScoredMetrics(std::string const& photoFilename) const;

private:
    pcontainer::Set const& photoSet;

    FuturePtr getFuture(std::string const& imageFilename) const;
    FuturePtr makeLoadingPromiseAndFuture(std::string const& imageFilename) const;
    QImage getPreloadNow(std::string const& imageFilename) const;

    void updateMetrics(std::string const& imageFilename, iprocess::MetricPtr const& metrics) const;
    void updateImage(std::string const& imageFilename, QImage const& image) const;

    using LookupKeyType = std::string;

    std::unordered_map<LookupKeyType, ConstPromisePtr> mutable promiseMap;

    std::unordered_map<LookupKeyType, const QImage> mutable preloadCache;
    std::unordered_map<LookupKeyType, const QImage> mutable imageCache;

    std::unordered_map<LookupKeyType, const iprocess::Metric> mutable metricCache;
    std::unordered_map<LookupKeyType, const iprocess::ScoredMetric> mutable scoredMetricCache;
};

}} // namespace phobos::icache

#endif // PHOBOS_IMAGECACHE_CACHE_H_
