#include <easylogging++.h>
#include <QThreadPool>
#include "PhotoContainers/Series.h"
#include "ImageCache/Cache.h"
#include "Utils/Algorithm.h"
#include "Utils/Preload.h"
#include "Utils/Asserted.h"
#include "ConfigExtension.h"
#include "ImageProcessing/MetricsAggregate.h"
#include "ImageProcessing/MetricsIO.h"

namespace phobos { namespace icache {

Cache::Cache(pcontainer::Set const& photoSet) :
    photoSet(photoSet)
{
}

namespace {
    QImage getInitialPreload()
    {
        static QImage const preloadImage =
            utils::preloadImage(config::qSize("imageCache.preloadSize", QSize(320, 240)));

        return preloadImage;
    }
} // unnamed namespace

QImage Cache::getImage(pcontainer::Item const& item) const
{
    auto it = imageCache.find(item.fileName());
    if (it != imageCache.end() && !it->second.full.isNull())
        return it->second.full;

    startThreadForItem(item);

    if (!it->second.preload.isNull())
        return it->second.preload;
    else
        return getInitialPreload();
}

QImage Cache::getPreload(pcontainer::Item const& item) const
{
    auto it = imageCache.find(item.fileName());
    if (it != imageCache.end() && !it->second.preload.isNull())
        return it->second.preload;

    startThreadForItem(item);
    return getInitialPreload();
}

std::unique_ptr<iprocess::LoaderThread> Cache::makeLoadingThread(std::string const& filename) const
{
    QSize const fullSize = config::qSize("imageCache.fullSize", QSize(1920, 1080));
    auto thread = std::make_unique<iprocess::LoaderThread>(filename, fullSize);

    thread->setAutoDelete(true);

    if (!hasMetrics(filename))
    {
        thread->withMetrics(hasMetrics(filename));
        QObject::connect(&thread->readySignals, &iprocess::LoaderThreadSignals::metricsReady,
                         this, &Cache::metricsReadyFromThread, Qt::QueuedConnection);
    }

    QObject::connect(&thread->readySignals, &iprocess::LoaderThreadSignals::imageReady,
                     this, &Cache::imageReadyFromThread, Qt::QueuedConnection);

    return thread;
}

void Cache::startThreadForItem(pcontainer::Item const& item) const
{
    if (utils::valueIn(item.fileName(), loadingImageSeriesId))
        return;

    loadingImageSeriesId.emplace(item.fileName(), item.seriesUuid());

    auto thread = makeLoadingThread(item.fileName());
    QThreadPool::globalInstance()->start(thread.release());
}

void Cache::imageReadyFromThread(QImage image, QString fileName)
{
  std::string stdFilename = fileName.toStdString();
  auto& entry = imageCache[stdFilename];
  entry.full = image;

  if (entry.preload.isNull())
  {
      auto const preloadSize = config::qSize("imageCache.preloadSize", QSize(320, 240));
      entry.preload = image.scaled(preloadSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  }

  auto const uuidIt = loadingImageSeriesId.find(stdFilename);
  assert(uuidIt != loadingImageSeriesId.end());

  QUuid const seriesId = uuidIt->second;
  loadingImageSeriesId.erase(uuidIt);

  emit updateImage(seriesId, fileName, image);
}

void Cache::metricsReadyFromThread(iprocess::MetricPtr metrics, QString fileName)
{
  std::string const stdFilename = fileName.toStdString();
  metricCache.emplace(stdFilename, metrics);

  auto const& seriesUuid = utils::asserted::fromMap(loadingImageSeriesId, stdFilename);
  auto const& series = photoSet.findSeries(seriesUuid);

  if (!std::all_of(series->begin(), series->end(),
              [this](pcontainer::ItemPtr const& item){
                  return utils::valueIn(item->fileName(), metricCache);
              }))
  {
      emit updateMetrics(seriesUuid, fileName, metrics);
  }

  auto allMetrics = utils::transformToVector<iprocess::MetricPtr>(series->begin(), series->end(),
      [this](auto const& item){ return metricCache[item->fileName()]; });

  iprocess::aggregateMetrics(allMetrics);

  bool const doLog = config::qualified("logging.metrics", false);
  for (std::size_t i = 0; i < series->size(); ++i)
  {
    std::string const& fn = series->item(i)->fileName();
    auto const& m = metricCache[fn];
    emit updateMetrics(seriesUuid, QString(fn.c_str()), m);

    LOG_IF(doLog, DEBUG) << "Calculated series metrics" << std::endl
       << "photoItem: " << fn << std::endl
       << "metric: " << m;
  }
}

bool Cache::hasMetrics(std::string const& photoFilename) const
{
    return utils::valueIn(photoFilename, metricCache);
}

iprocess::MetricPtr Cache::getMetrics(std::string const& photoFilename) const
{
    auto const it = metricCache.find(photoFilename);
    if (it == metricCache.end())
      return nullptr;

    return it->second;
}

}} // namespace phobos::icache
