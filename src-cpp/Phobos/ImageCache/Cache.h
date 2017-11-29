#ifndef PHOBOS_IMAGECACHE_CACHE_H_
#define PHOBOS_IMAGECACHE_CACHE_H_

#include "ImageCache/CacheFwd.h"
#include "ImageCache/MetricCache.h"
#include "ImageCache/LimitedMap.h"
#include "ImageCache/Transaction.h"
#include "ImageCache/LoadingManager.h"
#include "ImageCache/ProactiveScheduler.h"
#include "ImageProcessing/LoaderThread.h"
#include "PhotoContainers/Set.h"
#include "PhotoContainers/ItemId.h"
#include <QObject>
#include <QImage>
#include <QString>
#include <map>
#include <memory>

namespace phobos { namespace icache {

class Cache : public QObject
{
  Q_OBJECT

public:
  using LookupKeyType = QString;
  using ThumbnailCache = std::map<LookupKeyType, QImage>;

  explicit Cache(pcontainer::Set const& photoSet);

  Transaction transaction() { return Transaction(*this); }
  Result execute(Transaction && transaction);

  MetricCache const& metrics() const { return metricCache; }
  ThumbnailCache const& thumbnails() const { return thumbnailCache; }

public slots:
  void thumbnailReady(pcontainer::ItemId const& itemId, QImage const& image);
  void imageReady(pcontainer::ItemId const& itemId, QImage const& image);

signals:
  void updateMetrics(pcontainer::ItemId itemId, iprocess::metric::MetricPtr);

private slots:
  void changedSeries(QUuid const& seriesUuid);

private:
  friend class Transaction;

  Result executeImpl(Transaction const& transaction) const;

  ThumbnailCache thumbnailCache;
  LimitedMap fullImageCache;
  MetricCache metricCache;

  pcontainer::Set const& photoSet;
  LoadingManager loadingManager;
  ProactiveScheduler scheduler;
};

}} // namespace phobos::icache

#endif // PHOBOS_IMAGECACHE_CACHE_H_
