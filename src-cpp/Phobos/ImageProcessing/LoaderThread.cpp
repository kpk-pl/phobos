#include <algorithm>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <easylogging++.h>
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

namespace {
cv::Mat prepareForProcessing(cv::Mat cvImage)
{
    TIMED_FUNC(timedf);
    cv::cvtColor(cvImage, cvImage, cv::COLOR_BGR2GRAY);

    QSize const maxSize = config::qSize("imageLoaderThread.processingSize", QSize(1920, 1080));
    double const scale = 1.0/std::max(cvImage.cols/maxSize.width(), cvImage.rows/maxSize.height());
    cv::Mat resized;
    // TODO: test INTER_AREA
    // TODO: figure out dsize, do not pass scale
    cv::resize(cvImage, resized, cv::Size(), scale, scale, cv::INTER_CUBIC);
    return resized;
}
} // unnamed namespace

void LoaderThread::run()
{
    TIMED_FUNC(timedf);

    // TODO: if not calculate metrics then use plain QT to read image
    cv::Mat cvImage;
    {
        TIMED_SCOPE(scopef, "imread " + fileToLoad);
        cvImage = cv::imread(fileToLoad);
    }
    emitLoadedSignal(cvImage);

    if (calculateMetrics)
        runMetrics(prepareForProcessing(std::move(cvImage)));
}

void LoaderThread::runMetrics(cv::Mat cvImage) const
{
    TIMED_FUNC(timedf);

    MetricPtr metrics = std::make_shared<Metric>();

    {
        TIMED_SCOPE(scopef, "runMetrics: histogram");
        metrics->contrast = 0;
        metrics->histogram = normalizedHistogram(cvImage, *metrics->contrast);
    }
    {
        TIMED_SCOPE(scopef, "runMetrics: noise");
        metrics->noise = noiseMeasure(cvImage, config::qualified("imageLoaderThread.noiseMedianSize", 3));
    }
    {
        TIMED_SCOPE(scopef, "runMetrics: sobel");
        metrics->blur.sobel = blur::sobel(cvImage);
    }
    {
        TIMED_SCOPE(scopef, "runMetrics: laplace");
        metrics->blur.laplace = blur::laplace(cvImage);
    }
    {
        TIMED_SCOPE(scopef, "runMetrics: laplaceMod");
        metrics->blur.laplaceMod = blur::laplaceMod(cvImage);
    }

    emit readySignals.metricsReady(metrics);
}

void LoaderThread::emitLoadedSignal(cv::Mat const& cvImage) const
{
    TIMED_FUNC(timedf);
    // TODO: maybe scale in opencv and then convert to image -> CPU save?
    QPixmap pixmap = QPixmap::fromImage(iprocess::convCvToImage(cvImage));
    pixmap = iprocess::scalePixmap(pixmap, biggestClosestSize(pixmap.size()));
    auto pixmapPtr = std::make_shared<QPixmap>(std::move(pixmap));
    emit readySignals.pixmapReady(pixmapPtr);
}

QSize LoaderThread::biggestClosestSize(QSize const& pixmapSize) const
{
    std::vector<QSize> scaledSizes;
    std::vector<std::size_t> pixels;

    scaledSizes.reserve(requestedSizes.size());
    pixels.reserve(requestedSizes.size());

    for (auto const& size : requestedSizes)
    {
        int width = std::min(size.width(), pixmapSize.width());
        int height = pixmapSize.height() * width / pixmapSize.width();
        if (height > size.height())
        {
            height = size.height();
            width = pixmapSize.width() * height / pixmapSize.height();
        }

        scaledSizes.emplace_back(width, height);
        pixels.push_back(width*height);
    }

    auto const bestPixels = std::max_element(pixels.begin(), pixels.end());
    return scaledSizes[std::distance(pixels.begin(), bestPixels)];
}

}} // namespace phobos::iprocess
