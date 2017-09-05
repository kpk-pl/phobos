#ifndef PHOBOS_IMAGECACHE_CACHE_H_
#define PHOBOS_IMAGECACHE_CACHE_H_

#include "ImageCache/CacheFwd.h"
#include "ImageCache/MetricCache.h"
#include "ImageCache/LimitedMap.h"
#include "ImageCache/PriorityThreadPool.h"
#include "ImageProcessing/LoaderThread.h"
#include "PhotoContainers/Set.h"
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

  std::map<pcontainer::ItemId, QImage> getImages(QUuid const& seriesId);
  std::map<pcontainer::ItemId, QImage> getThumbnails(QUuid const& seriesId);

  QImage getImage(pcontainer::ItemId const& itemId);
  QImage getThumbnail(pcontainer::ItemId const& itemId);

  MetricCache const& metrics() const { return metricCache; }

signals:
  void updateImage(pcontainer::ItemId itemId, QImage image);
  void updateMetrics(pcontainer::ItemId itemId, iprocess::MetricPtr);

private slots:
  void imageReadyFromThread(pcontainer::ItemId itemId, QImage image);

private:
  using LookupKeyType = QString;
  class Transaction;

  std::unique_ptr<iprocess::LoaderThread> makeLoadingThread(pcontainer::ItemId const& itemId) const;
  void startThreadForItem(pcontainer::ItemId const& itemId);

  std::set<LookupKeyType> alreadyLoading;

  std::map<LookupKeyType, QImage> thumbnailCache;
  LimitedMap fullImageCache;

  MetricCache metricCache;
  pcontainer::Set const& photoSet;
  PriorityThreadPool threadPool;
};

}} // namespace phobos::icache

#endif // PHOBOS_IMAGECACHE_CACHE_H_
