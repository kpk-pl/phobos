#include "ImageProcessing/FormatConversion.h"
#include "ImageProcessing/ScalePixmap.h"
#include "ImageProcessing/LoaderThread.h"
#include "ImageProcessing/Bluriness.h"
#include "ImageProcessing/Noisiness.h"
#include "ImageProcessing/Histogram.h"
#include "ConfigExtension.h"
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

LoaderThread::LoaderThread(pcontainer::ItemId const& itemId, QSize const& requestedSize) :
    itemId(itemId), requestedSize(requestedSize)
{
}

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
}

// TODO: create managed threadpool
// 1. Enable removing unneeded jobs from queue when opening another series view
// 2. Higher priority for loading series views than for loading all-series views
// 3. Metric calculation is pretty slow -> lowest priority on one separate thread in background
// Maybe divide threadpool into dedicated threads: 1 for metrics, 1 for all-view, 2 for seriesview
// Promote threads to other run-queues when they are empty
// AKA scheduler
//
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

void LoaderThread::runMetrics(cv::Mat cvImage) const
{
    TIMED_FUNC(scopefunc);

    QSize const maxSize = config::qSize("imageLoaderThread.processingSize", QSize(1920, 1080));
    QSize const scaledSize = QSize(cvImage.cols, cvImage.rows).scaled(maxSize, Qt::KeepAspectRatio);
    LOG(DEBUG) << "Scaling image for metrics from " << cvImage.cols << "x" << cvImage.rows
               << " to " << scaledSize.width() << "x" << scaledSize.height();

    cv::Mat resized;
    cv::resize(cvImage, resized, cv::Size(scaledSize.width(), scaledSize.height()), 0, 0, cv::INTER_CUBIC);
    cvImage.release();

    MetricPtr metrics = std::make_shared<Metric>();

    {
      std::vector<cv::Mat> bgrPlanes;
      cv::split(resized, bgrPlanes);
      assert(bgrPlanes.size() == 3);
      TIMED("runMetrics: blueHistogram", metrics->histogram.data.emplace(Histogram::Channel::Blue, normalizedHistogram(bgrPlanes[0], nullptr)));
      TIMED("runMetrics: greenHistogram", metrics->histogram.data.emplace(Histogram::Channel::Green, normalizedHistogram(bgrPlanes[1], nullptr)));
      TIMED("runMetrics: redHistogram", metrics->histogram.data.emplace(Histogram::Channel::Red, normalizedHistogram(bgrPlanes[2], nullptr)));
    }

    cv::cvtColor(resized, resized, cv::COLOR_BGR2GRAY);

    metrics->contrast = 0;
    TIMED("runMetrics: contrast", metrics->histogram.data.emplace(Histogram::Channel::Value, normalizedHistogram(resized, &*metrics->contrast)));

    TIMED("runMetrics: noise", metrics->noise = noiseMeasure(resized, config::qualified("imageLoaderThread.noiseMedianSize", 3)));
    TIMED("runMetrics: sobel", metrics->blur.sobel = blur::sobel(resized));
    TIMED("runMetrics: laplace", metrics->blur.laplace = blur::laplace(resized));
    TIMED("runMetrics: laplaceMod", metrics->blur.laplaceMod = blur::laplaceMod(resized));

    emit readySignals.metricsReady(itemId, metrics);
}

}} // namespace phobos::iprocess
