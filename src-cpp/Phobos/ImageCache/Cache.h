#ifndef PHOBOS_IMAGECACHE_CACHE_H_
#define PHOBOS_IMAGECACHE_CACHE_H_

#include "ImageCache/CacheFwd.h"
#include "ImageCache/MetricCache.h"
#include "ImageCache/LimitedMap.h"
#include "ImageCache/PriorityThreadPool.h"
#include "ImageCache/Transaction.h"
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
  explicit Cache(pcontainer::Set const& photoSet);

  Transaction transaction() { return Transaction(*this); }
  Transaction::Result execute(Transaction && transaction);

  MetricCache const& metrics() const { return metricCache; }

signals:
  void updateMetrics(pcontainer::ItemId itemId, iprocess::metric::MetricPtr);

private slots:
  void imageReadyFromThread(pcontainer::ItemId itemId, QImage image);
  void changedSeries(QUuid const& seriesUuid);

private:
  friend class Transaction;
  using LookupKeyType = QString;

  std::unique_ptr<iprocess::LoaderThread> makeLoadingThread(pcontainer::ItemId const& itemId) const;
  void startThreadForItem(Transaction && transaction);

  std::multimap<pcontainer::ItemId, std::pair<Runnable::UniqueId, Transaction>> transactionsInThread;

  std::map<LookupKeyType, QImage> thumbnailCache;
  LimitedMap fullImageCache;

  MetricCache metricCache;
  pcontainer::Set const& photoSet;
  PriorityThreadPool threadPool;
};

}} // namespace phobos::icache

#endif // PHOBOS_IMAGECACHE_CACHE_H_
