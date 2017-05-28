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
    void makeNewPromise(std::string const& imageFilename) const;
    QImage getInitialPreload() const;

    void updateMetrics(std::string const& imageFilename, iprocess::MetricPtr const& metrics) const;

    using LookupKeyType = std::string;

    std::unordered_map<LookupKeyType, PromisePtr> mutable imagePromiseMap;
    std::unordered_map<LookupKeyType, const iprocess::Metric> mutable metricCache;
    std::unordered_map<LookupKeyType, const iprocess::ScoredMetric> mutable scoredMetricCache;
};

}} // namespace phobos::icache

#endif // PHOBOS_IMAGECACHE_CACHE_H_