#include "ImageProcessing/FormatConversion.h"
#include "ImageProcessing/ScalePixmap.h"
#include "ImageProcessing/LoaderThread.h"
#include "ImageProcessing/Bluriness.h"
#include "ImageProcessing/Noisiness.h"
#include "ImageProcessing/Histogram.h"
#include "ImageProcessing/Sharpness.h"
#include "ConfigExtension.h"
#include "ConfigPath.h"
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <easylogging++.h>
#include <QImageReader>
#include <algorithm>
#include <utility>

//#define PHOBOS_TIME_IMAGE_OPS

#ifdef PHOBOS_TIME_IMAGE_OPS
#define TIMED(x, op) do { TIMED_SCOPE(timed_scope, x); op; } while(0)
#else
#define TIMED(x, op) do { op; } while(0)
#endif

namespace phobos { namespace iprocess {

namespace {
config::ConfigPath const configPath("imageLoaderThread");
} // unnamed namespace

LoaderThread::LoaderThread(pcontainer::ItemId const& itemId, QSize const& requestedSize) :
    itemId(itemId), requestedSize(requestedSize)
{}

icache::Runnable::Id LoaderThread::id() const
{
  return std::hash<std::string>{}(itemId.toString());
}

// TODO: optimize with-metrics flow

void LoaderThread::run()
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
}

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

namespace {
cv::Mat prepareForProcessing(cv::Mat && cvImage)
{
  QSize const maxSize = config::qSize(configPath("processingSize"), QSize(1920, 1080));
  QSize const scaledSize = QSize(cvImage.cols, cvImage.rows).scaled(maxSize, Qt::KeepAspectRatio);
  LOG(DEBUG) << "Scaling image for metrics from " << cvImage.cols << "x" << cvImage.rows
             << " to " << scaledSize.width() << "x" << scaledSize.height();

  cv::Mat resized;
  cv::resize(cvImage, resized, cv::Size(scaledSize.width(), scaledSize.height()), 0, 0, cv::INTER_CUBIC);

  return resized;
}

void calculateColorHistograms(cv::Mat const& cvImage, Metric &metrics)
{
  std::vector<cv::Mat> bgrPlanes;
  cv::split(cvImage, bgrPlanes);
  assert(bgrPlanes.size() == 3);

  TIMED("runMetrics: blueHistogram", metrics.histogram.data.emplace(Histogram::Channel::Blue, normalizedHistogram(bgrPlanes[0], nullptr)));
  TIMED("runMetrics: greenHistogram", metrics.histogram.data.emplace(Histogram::Channel::Green, normalizedHistogram(bgrPlanes[1], nullptr)));
  TIMED("runMetrics: redHistogram", metrics.histogram.data.emplace(Histogram::Channel::Red, normalizedHistogram(bgrPlanes[2], nullptr)));
}

void calculateColorfullFeatures(cv::Mat const& cvImage, Metric &metrics)
{
  calculateColorHistograms(cvImage, metrics);
}

void calculateGrayscaleFeatures(cv::Mat const& cvImage, Metric &metrics)
{
  metrics.contrast = 0;
  TIMED("runMetrics: contrast", metrics.histogram.data.emplace(Histogram::Channel::Value, normalizedHistogram(cvImage, &*metrics.contrast)));

  TIMED("runMetrics: noise", metrics.noise = noiseMeasure(cvImage, config::qualified(configPath("noiseMedianSize"), 3)));
  TIMED("tunMetrics: sharpness", metrics.sharpness = sharpness::homogeneous(cvImage, 5));

  std::string const blurAlgo = config::qualified(configPath("blurAlgorithm"), std::string("laplace"));
  if (blurAlgo == "sobel")
    TIMED("runMetrics: sobel", metrics.blur = blur::sobel(cvImage));
  else if (blurAlgo == "laplaceMod")
    TIMED("runMetrics: laplaceMod", metrics.blur = blur::laplaceMod(cvImage));
  else
    TIMED("runMetrics: laplace", metrics.blur = blur::laplace(cvImage));
}
} // unnamed namespace

void LoaderThread::runMetrics(cv::Mat cvImage) const
{
  TIMED_FUNC(scopefunc);

  cvImage = prepareForProcessing(std::move(cvImage));
  MetricPtr metrics = std::make_shared<Metric>();

  calculateColorfullFeatures(cvImage, *metrics);
  cv::cvtColor(cvImage, cvImage, cv::COLOR_BGR2GRAY);
  calculateGrayscaleFeatures(cvImage, *metrics);

  emit readySignals.metricsReady(itemId, metrics);
}

}} // namespace phobos::iprocess
