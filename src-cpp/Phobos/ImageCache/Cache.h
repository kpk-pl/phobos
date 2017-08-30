#ifndef PHOBOS_IMAGECACHE_CACHE_H_
#define PHOBOS_IMAGECACHE_CACHE_H_

#include "ImageCache/CacheFwd.h"
#include "ImageCache/LimitedMap.h"
#include "PhotoContainers/Set.h"
#include "ImageProcessing/LoaderThread.h"
#include "PhotoContainers/ItemId.h"
#include <QObject>
#include <QImage>
#include <QString>
#include <map>
#include <set>
#include <memory>

namespace phobos { namespace icache {

class Cache : public QObject
{
  Q_OBJECT

public:
  explicit Cache(pcontainer::Set const& photoSet);

  std::map<pcontainer::ItemId, QImage> getImages(QUuid const& seriesId) const;
  std::map<pcontainer::ItemId, QImage> getThumbnails(QUuid const& seriesId) const;

  QImage getImage(pcontainer::ItemId const& itemId) const;
  QImage getThumbnail(pcontainer::ItemId const& itemId) const;

  bool hasMetrics(pcontainer::ItemId const& itemId) const;
  iprocess::MetricPtr getMetrics(pcontainer::ItemId const& itemId) const;

signals:
  void updateImage(pcontainer::ItemId itemId, QImage image);
  void updateMetrics(pcontainer::ItemId itemId, iprocess::MetricPtr);

private slots:
  void imageReadyFromThread(pcontainer::ItemId itemId, QImage image);
  void metricsReadyFromThread(pcontainer::ItemId itemId, iprocess::MetricPtr image);
  bool updateSeriesMetrics(QUuid const& seriesUuid);

// TODO: get rid of those mutables
//
// Split into image cache and metrics cache
// cache should contain both of them
private:
  pcontainer::Set const& photoSet;

  QImage getImageWithLoading(pcontainer::ItemId const& itemId) const;
  QImage getThumbnailWithLoading(pcontainer::ItemId const& itemId, bool requestLoad) const;
  std::unique_ptr<iprocess::LoaderThread> makeLoadingThread(pcontainer::ItemId const& itemId) const;
  void startThreadForItem(pcontainer::ItemId const& itemId) const;

  using LookupKeyType = QString;

  // if image is in this map, it is already loading
  std::set<LookupKeyType> mutable alreadyLoading;

  std::map<LookupKeyType, QImage> mutable thumbnailCache;
  LimitedMap mutable fullImageCache;
  std::map<LookupKeyType, iprocess::MetricPtr> mutable metricCache;
};

}} // namespace phobos::icache

#endif // PHOBOS_IMAGECACHE_CACHE_H_
