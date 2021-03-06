#ifndef IMAGECACHE_LOADINGMANAGER_H
#define IMAGECACHE_LOADINGMANAGER_H

#include "ImageCache/PriorityThreadPool.h"
#include "ImageCache/CacheFwd.h"
#include "ImageCache/TransactionFwd.h"
#include "PhotoContainers/ItemId.h"
#include "ImageProcessing/LoaderThread.h"
#include <QObject>

namespace phobos { namespace icache {

class LoadingManager : public QObject
{
  Q_OBJECT

public:
  explicit LoadingManager(Cache const& cache);

  void start(ConstTransactionPtrVec && schedule);
  void stop(pcontainer::ItemId const& itemId);

signals:
  void thumbnailReady(pcontainer::ItemId const& itemId, QImage const& image);
  void imageReady(pcontainer::ItemId const& itemId, QImage const& image, Priority const generation);
  void metricsReady(pcontainer::ItemId const& itemId, iprocess::MetricPtr metrics);

private slots:
  void imageLoaded(pcontainer::ItemId const& itemId, QImage const& image);

private:
  std::multimap<pcontainer::ItemId, std::pair<Runnable::UniqueId, ConstTransactionPtr>> jobsInThread;

  void startOne(ConstTransactionPtr && transaction);
  std::unique_ptr<iprocess::LoaderThread> makeLoadingThread(pcontainer::ItemId const& itemId) const;

  PriorityThreadPool threadPool;
  Cache const& cache;
};

}} // namespace phobos::icache

#endif // IMAGECACHE_LOADINGMANAGER_H
