#ifndef PHOBOS_IMAGECACHE_CACHE_H_
#define PHOBOS_IMAGECACHE_CACHE_H_

#include <unordered_map>
#include <list>
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

public:
    explicit Cache(pcontainer::Set const& photoSet);

    QImage getImage(pcontainer::Item const& item) const;
    QImage getPreload(pcontainer::Item const& item) const;

    bool hasMetrics(std::string const& photoFilename) const;
    iprocess::MetricPtr getMetrics(std::string const& photoFilename) const;

signals:
    void updateImage(QUuid seriesUuid, QString filename, QImage image);
    void updateMetrics(QUuid seriesUuid, QString filename, iprocess::MetricPtr);

private slots:
    void imageReadyFromThread(QImage image, QString fileName);
    void metricsReadyFromThread(iprocess::MetricPtr image, QString fileName);

// TODO: get rid of those mutables
//
// Split into image cache and metrics cache
// cache should contain both of them
private:
    pcontainer::Set const& photoSet;

    std::unique_ptr<iprocess::LoaderThread> makeLoadingThread(std::string const& filename) const;
    void startThreadForItem(pcontainer::Item const& item) const;
    void insertToFullCache(QImage const& image, std::string const& filename) const;

    using LookupKeyType = std::string;

    // if image is in this map, it is already loading
    std::unordered_map<LookupKeyType, QUuid> mutable loadingImageSeriesId;

    std::unordered_map<LookupKeyType, QImage> mutable preloadImageCache;
    std::unordered_map<LookupKeyType, QImage> mutable fullImageCache;
    std::unordered_map<LookupKeyType, iprocess::MetricPtr> mutable metricCache;

    std::size_t mutable fullCacheSize = 0;
    std::list<LookupKeyType> mutable fullImageLastAccess;
};

}} // namespace phobos::icache

#endif // PHOBOS_IMAGECACHE_CACHE_H_
