#include <algorithm>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <easylogging++.h>
#include <QImageReader>
#include "ImageProcessing/FormatConversion.h"
#include "ImageProcessing/ScalePixmap.h"
#include "ImageProcessing/LoaderThread.h"
#include "ImageProcessing/Bluriness.h"
#include "ImageProcessing/Noisiness.h"
#include "ImageProcessing/Histogram.h"
#include "ConfigExtension.h"

//#define PHOBOS_TIME_IMAGE_OPS

#ifdef PHOBOS_TIME_IMAGE_OPS
#define TIMED(x, op) do { TIMED_SCOPE(timed_scope, x); op; } while(0)
#else
#define TIMED(x, op) do { op; } while(0)
#endif

namespace phobos { namespace iprocess {

LoaderThread::LoaderThread(std::string const& fileName,
                           std::vector<QSize> const& requestedSizes,
                           bool calculateMetrics) :
    fileToLoad(fileName),
    requestedSizes(requestedSizes),
    calculateMetrics(calculateMetrics)
{
}

// TODO: optimize with-metrics flow

void LoaderThread::run()
{
    TIMED_FUNC(scopefunc);
    if (calculateMetrics)
    {
        cv::Mat cvImage;
        {
            TIMED("load:imread", cvImage = cv::imread(fileToLoad));
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

void LoaderThread::runWithoutMetrics() const
{
    TIMED_FUNC(id);

    QImageReader reader(fileToLoad.c_str());
    reader.setAutoTransform(true);
    reader.setAutoDetectImageFormat(true);
    reader.setScaledSize(biggestClosestSize(reader.size()));

    QImage image;
    TIMED("QImageReade:read", image = reader.read());

    emit readySignals.imageReady(image, fileToLoad);
}

void LoaderThread::emitLoadedSignal(cv::Mat const& cvImage)
{
    QSize const cvSize(cvImage.cols, cvImage.rows);
    QSize const pixmapSize = biggestClosestSize(cvSize);
    LOG(DEBUG) << "Scaling image from " << cvImage.cols << "x" << cvImage.rows
               << " to " << pixmapSize.width() << "x" << pixmapSize.height();

    cv::Mat resized;
    TIMED("cv:resize", cv::resize(cvImage, resized, cv::Size(pixmapSize.width(), pixmapSize.height()), 0, 0, cv::INTER_CUBIC));

    QImage image;
    TIMED("cv:convQt", image = iprocess::convCvToImage(resized));

    emit readySignals.imageReady(image, fileToLoad);
}

// TODO optimize double scaling when calculating metrics
void LoaderThread::runMetrics(cv::Mat cvImage) const
{
    TIMED_FUNC(scopefunc);
    cv::cvtColor(cvImage, cvImage, cv::COLOR_BGR2GRAY);

    QSize const maxSize = config::qSize("imageLoaderThread.processingSize", QSize(1920, 1080));
    QSize const scaledSize = QSize(cvImage.cols, cvImage.rows).scaled(maxSize, Qt::KeepAspectRatio);
    LOG(DEBUG) << "Scaling image for metrics from " << cvImage.cols << "x" << cvImage.rows
               << " to " << scaledSize.width() << "x" << scaledSize.height();

    cv::Mat resized;
    cv::resize(cvImage, resized, cv::Size(scaledSize.width(), scaledSize.height()), 0, 0, cv::INTER_CUBIC);
    cvImage.release();

    MetricPtr metrics = std::make_shared<Metric>();

    metrics->contrast = 0;
    TIMED("runMetrics: contrast", metrics->histogram = normalizedHistogram(resized, *metrics->contrast));
    TIMED("runMetrics: noise", metrics->noise = noiseMeasure(resized, config::qualified("imageLoaderThread.noiseMedianSize", 3)));
    TIMED("runMetrics: sobel", metrics->blur.sobel = blur::sobel(resized));
    TIMED("runMetrics: laplace", metrics->blur.laplace = blur::laplace(resized));
    TIMED("runMetrics: laplaceMod", metrics->blur.laplaceMod = blur::laplaceMod(resized));

    emit readySignals.metricsReady(metrics, fileToLoad);
}

QSize LoaderThread::biggestClosestSize(QSize const& pixmapSize) const
{
    std::vector<QSize> scaledSizes;
    std::vector<std::size_t> pixels;

    scaledSizes.reserve(requestedSizes.size());
    pixels.reserve(requestedSizes.size());

    for (auto const& size : requestedSizes)
    {
        QSize const scaled = pixmapSize.scaled(size, Qt::KeepAspectRatio);
        scaledSizes.push_back(scaled);
        pixels.push_back(scaled.width() * scaled.height());
    }

    auto const bestPixels = std::max_element(pixels.begin(), pixels.end());
    return scaledSizes[std::distance(pixels.begin(), bestPixels)];
}

}} // namespace phobos::iprocess
