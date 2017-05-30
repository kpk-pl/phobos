#ifndef PHOBOS_IMAGECACHE_CACHE_H_
#define PHOBOS_IMAGECACHE_CACHE_H_

#include <unordered_map>
#include <stdexcept>
#include <string>
#include <memory>
#include <QObject>
#include <QImage>
#include "ImageCache/CacheFwd.h"
#include "PhotoContainers/Set.h"
#include "ImageProcessing/LoaderThread.h"

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

    QImage getImage(pcontainer::Item const& item) const;
    QImage getPreload(pcontainer::Item const& item) const;

    bool hasMetrics(std::string const& photoFilename) const;
    bool hasScoredMetrics(std::string const& photoFilename) const;
    iprocess::Metric const& getMetrics(std::string const& photoFilename) const;
    iprocess::ScoredMetric const& getScoredMetrics(std::string const& photoFilename) const;

signals:
    void updateImage(QUuid seriesUuid, std::string filename, QImage image);

private slots:
    void imageReadyFromThread(QImage image, std::string fileName);
    void metricsReadyFromThread(iprocess::MetricPtr image, std::string fileName);

private:
    pcontainer::Set const& photoSet;

    std::unique_ptr<iprocess::LoaderThread> makeLoadingThread(std::string const& filename) const;
    void startThreadForItem(pcontainer::Item const& item) const;

    using LookupKeyType = std::string;
    struct CachedType {
        QImage preload;
        QImage full;
    };

    // if image is in this map, it is already loading
    std::unordered_map<LookupKeyType, QUuid> mutable loadingImageSeriesId;

    std::unordered_map<LookupKeyType, CachedType> mutable imageCache;
    std::unordered_map<LookupKeyType, const iprocess::Metric> mutable metricCache;
    std::unordered_map<LookupKeyType, const iprocess::ScoredMetric> mutable scoredMetricCache;
};

}} // namespace phobos::icache

#endif // PHOBOS_IMAGECACHE_CACHE_H_
