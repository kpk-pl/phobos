#ifndef IMAGECACHE_METRIC_CACHE_H
#define IMAGECACHE_METRIC_CACHE_H

#include "PhotoContainers/ItemId.h"
#include "PhotoContainers/Fwd.h"
#include "ImageProcessing/MetricsFwd.h"
#include <QObject>
#include <QString>

namespace phobos { namespace icache {

class MetricCache : public QObject
{
  Q_OBJECT

public:
  explicit MetricCache(pcontainer::Set const& photoSet);

  bool has(pcontainer::ItemId const& itemId) const;
  iprocess::metric::MetricPtr get(pcontainer::ItemId const& itemId) const;

signals:
  void updateMetrics(pcontainer::ItemId itemId, iprocess::metric::MetricPtr);

public slots:
  void newLoadedFromThread(pcontainer::ItemId itemId, iprocess::metric::MetricPtr image);

private slots:
  bool changedSeries(QUuid const& seriesUuid);

private:
  using LookupKeyType = QString;
  std::map<LookupKeyType, iprocess::metric::MetricPtr> metricCache;
  pcontainer::Set const& photoSet;
};

}} // namespace phobos::icache

#endif // IMAGECACHE_METRIC_CACHE_H
