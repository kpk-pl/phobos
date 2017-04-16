#include <algorithm>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <easylogging++.h>
#include <QImage>
#include <QImageReader>
#include "ImageProcessing/FormatConversion.h"
#include "ImageProcessing/ScalePixmap.h"
#include "ImageProcessing/LoaderThread.h"
#include "ImageProcessing/Bluriness.h"
#include "ImageProcessing/Noisiness.h"
#include "ImageProcessing/Histogram.h"
#include "ConfigExtension.h"

namespace phobos { namespace iprocess {

LoaderThread::LoaderThread(std::string const& fileName,
                           std::vector<QSize> const& requestedSizes,
                           bool calculateMetrics) :
    fileToLoad(fileName),
    requestedSizes(requestedSizes),
    calculateMetrics(calculateMetrics)
{
}

void LoaderThread::run()
{
    TIMED_FUNC(scopefunc);
    if (calculateMetrics)
    {
        cv::Mat cvImage;
        {
            TIMED_SCOPE(scopeWithMetric, "LoaderThread::loadInOpenCV");
            cvImage = cv::imread(fileToLoad);
            emitLoadedSignal(QPixmap::fromImage(iprocess::convCvToImage(cvImage)));
        }

        runMetrics(std::move(cvImage));
    }
    else
    {
        TIMED_SCOPE(scopeNoMetric, "LoaderThread::loadInQt");
        QImageReader reader(fileToLoad.c_str());
        reader.setAutoTransform(true);
        reader.setAutoDetectImageFormat(true);
        emitLoadedSignal(QPixmap::fromImageReader(&reader));
    }
}

void LoaderThread::emitLoadedSignal(QPixmap pixmap)
{
    // TODO: maybe scale in opencv and then convert to image -> CPU save?
    QSize const pixmapSize = biggestClosestSize(pixmap.size());
    LOG(DEBUG) << "Scaling pixmap from " << pixmap.width() << "x" << pixmap.height()
               << " to " << pixmapSize.width() << "x" << pixmapSize.height();
    pixmap = iprocess::scalePixmap(pixmap, pixmapSize);
    emit readySignals.pixmapReady(std::make_shared<QPixmap>(std::move(pixmap)));
}

void LoaderThread::runMetrics(cv::Mat cvImage) const
{
    TIMED_FUNC(scopefunc);
    cv::cvtColor(cvImage, cvImage, cv::COLOR_BGR2GRAY);

    QSize const maxSize = config::qSize("imageLoaderThread.processingSize", QSize(1920, 1080));
    QSize const scaledSize = QSize(cvImage.cols, cvImage.rows).scaled(maxSize, Qt::KeepAspectRatio);
    LOG(DEBUG) << "Scaling image from " << cvImage.cols << "x" << cvImage.rows
               << " to " << scaledSize.width() << "x" << scaledSize.height();

    cv::Mat resized;
    // TODO: test INTER_AREA
    cv::resize(cvImage, resized, cv::Size(scaledSize.width(), scaledSize.height()), 0, 0, cv::INTER_CUBIC);
    cvImage.release();

    MetricPtr metrics = std::make_shared<Metric>();

    {
        TIMED_SCOPE(scopef, "runMetrics: histogram");
        metrics->contrast = 0;
        metrics->histogram = normalizedHistogram(resized, *metrics->contrast);
    }
    {
        TIMED_SCOPE(scopef, "runMetrics: noise");
        metrics->noise = noiseMeasure(resized, config::qualified("imageLoaderThread.noiseMedianSize", 3));
    }
    {
        TIMED_SCOPE(scopef, "runMetrics: sobel");
        metrics->blur.sobel = blur::sobel(resized);
    }
    {
        TIMED_SCOPE(scopef, "runMetrics: laplace");
        metrics->blur.laplace = blur::laplace(resized);
    }
    {
        TIMED_SCOPE(scopef, "runMetrics: laplaceMod");
        metrics->blur.laplaceMod = blur::laplaceMod(resized);
    }

    emit readySignals.metricsReady(metrics);
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
