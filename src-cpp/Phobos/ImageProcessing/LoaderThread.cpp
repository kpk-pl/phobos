#include "ImageProcessing/FormatConversion.h"
#include "ImageProcessing/ScalePixmap.h"
#include "ImageProcessing/LoaderThread.h"
#include "ImageProcessing/MetricCalculator.h"
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <easylogging++.h>
#include <QImageReader>
#include <utility>

//#define PHOBOS_TIME_IMAGE_OPS

#ifdef PHOBOS_TIME_IMAGE_OPS
#define TIMED(x, op) do { TIMED_SCOPE(timed_scope, x); op; } while(0)
#else
#define TIMED(x, op) do { op; } while(0)
#endif

namespace phobos { namespace iprocess {

LoaderThread::LoaderThread(pcontainer::ItemId const& itemId, QSize const& requestedSize) :
    itemId(itemId), requestedSize(requestedSize)
{}

icache::Runnable::Id LoaderThread::id() const
{
  return std::hash<std::string>{}(itemId.toString());
}

// TODO: optimize with-metrics flow

void LoaderThread::runImpl()
{
  TIMED_FUNC(scopefunc);
  if (calculateMetrics)
  {
    cv::Mat cvImage;
    {
      TIMED("load:imread", cvImage = cv::imread(itemId.fileName.toStdString().c_str()));
      // if (!cvImage.data) ...
      emitLoadedSignal(cvImage);
    }
    runMetrics(std::move(cvImage));
  }
  else
  {
    runWithoutMetrics();
  }

  emit signal.finished(id());
}

// TODO: configurable options for metric calculations, weights, disable some portions of calculations

// TODO: Use transaction-like logic with unique IDs to display INFO information about loaded images

// TODO: BUG: handle nullptrs and QImage.isNull() == true. Probably sometimes it is impossible to allocate more memory and nullpts are used
// If this thread crashes, whole app crashes.
// To reproduce try to load A LOT of phtoos with increased cache size. Huge cache size helps to reproduce.

namespace {
  QSize scaledDown(QSize const& size, QSize const& limit)
  {
    QSize const dummyScale = size.scaled(limit, Qt::KeepAspectRatio);
    if (dummyScale.width() > size.width() || dummyScale.height() > size.height())
      return size;
    return dummyScale;
  }
} // unnamed namespace

void LoaderThread::runWithoutMetrics() const
{
  TIMED_FUNC(id);

  QImageReader reader(itemId.fileName.toStdString().c_str());
  reader.setAutoTransform(true);
  reader.setAutoDetectImageFormat(true);

  QSize const scaledSize = scaledDown(reader.size(), requestedSize);
  LOG(DEBUG) << "Reading image " << itemId.fileName
             << " with size: " << reader.size().width() << "x" << reader.size().height()
             << " scaled to " << scaledSize.width() << "x" << scaledSize.height();
  reader.setScaledSize(scaledSize);

  QImage image;
  TIMED("QImageReade:read", image = reader.read());

  emit readySignals.imageReady(itemId, image);
}

void LoaderThread::emitLoadedSignal(cv::Mat const& cvImage)
{
  QSize const cvSize(cvImage.cols, cvImage.rows);
  QSize const pixmapSize = scaledDown(cvSize, requestedSize);
  LOG(DEBUG) << "Scaling " << itemId.fileName << " from " << cvImage.cols << "x" << cvImage.rows
             << " to " << pixmapSize.width() << "x" << pixmapSize.height();

  cv::Mat resized;
  TIMED("cv:resize", cv::resize(cvImage, resized, cv::Size(pixmapSize.width(), pixmapSize.height()), 0, 0, cv::INTER_CUBIC));

  QImage image;
  TIMED("cv:convQt", image = iprocess::convCvToImage(resized));

  emit readySignals.imageReady(itemId, image);
}

void LoaderThread::runMetrics(cv::Mat cvImage) const
{
  TIMED_FUNC(scopefunc);
  emit readySignals.metricsReady(itemId, calcMetrics(std::move(cvImage)));
}

}} // namespace phobos::iprocess
