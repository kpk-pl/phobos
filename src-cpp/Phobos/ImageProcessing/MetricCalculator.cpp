#include "ImageProcessing/MetricCalculator.h"
#include "ImageProcessing/Histogram.h"
#include "ImageProcessing/Bluriness.h"
#include "ImageProcessing/Noisiness.h"
#include "ImageProcessing/Sharpness.h"
#include "ImageProcessing/Metrics.h"
#include "ConfigExtension.h"
#include "ConfigPath.h"
#include <easylogging++.h>
#include <opencv2/opencv.hpp>
#include <QSize>

//#define PHOBOS_TIME_IMAGE_OPS

#ifdef PHOBOS_TIME_IMAGE_OPS
#define TIMED(x, op) do { TIMED_SCOPE(timed_scope, x); op; } while(0)
#else
#define TIMED(x, op) do { op; } while(0)
#endif

namespace phobos { namespace iprocess {

namespace {
config::ConfigPath const configPath("metricCalculator");

class Processor
{
public:
  Processor(cv::Mat const& image) :
    metrics(std::make_shared<metric::Metric>())
  {
    QSize const maxSize = config::qSize(configPath("processingSize"), QSize(1920, 1080));
    QSize const scaledSize = QSize(image.cols, image.rows).scaled(maxSize, Qt::KeepAspectRatio);
    LOG(DEBUG) << "Scaling image for metrics from " << image.cols << "x" << image.rows
               << " to " << scaledSize.width() << "x" << scaledSize.height();

    cv::resize(image, baseImage, cv::Size(scaledSize.width(), scaledSize.height()), 0, 0, cv::INTER_CUBIC);
  }

  metric::MetricPtr result() const
  {
    return metrics;
  }

  void toGrayscale()
  {
    cv::cvtColor(baseImage, baseImage, cv::COLOR_BGR2GRAY);
  }

  void equalize()
  {
    assert(baseImage.channels() == 1);
    cv::Mat result;
    cv::equalizeHist(baseImage, result);
    baseImage = result;
  }

  void colorHistograms()
  {
    std::vector<cv::Mat> bgrPlanes;
    cv::split(baseImage, bgrPlanes);
    assert(bgrPlanes.size() == 3);

    using Channel = metric::Histogram::Channel;

    TIMED("runMetrics: blueHistogram", metrics->histogram.data.emplace(Channel::Blue, normalizedHistogram(bgrPlanes[0], nullptr)));
    TIMED("runMetrics: greenHistogram", metrics->histogram.data.emplace(Channel::Green, normalizedHistogram(bgrPlanes[1], nullptr)));
    TIMED("runMetrics: redHistogram", metrics->histogram.data.emplace(Channel::Red, normalizedHistogram(bgrPlanes[2], nullptr)));
  }

  void histogram()
  {
    metrics->contrast = 0;
    using Channel = metric::Histogram::Channel;
    TIMED("runMetrics: contrast", metrics->histogram.data.emplace(Channel::Value, normalizedHistogram(baseImage, &*metrics->contrast)));
  }

  void noise()
  {
    TIMED("runMetrics: noise", metrics->noise = noiseMeasure(baseImage, config::qualified(configPath("noiseMedianSize"), 3)));
  }

  void blur()
  {
    std::string const blurAlgo = config::qualified(configPath("blurAlgorithm"), std::string("laplace"));
    if (blurAlgo == "sobel")
      TIMED("runMetrics: sobel", metrics->blur = blur::sobel(baseImage));
    else if (blurAlgo == "laplaceMod")
      TIMED("runMetrics: laplaceMod", metrics->blur = blur::laplaceMod(baseImage));
    else
      TIMED("runMetrics: laplace", metrics->blur = blur::laplace(baseImage));
  }

  void sharpness()
  {
    sharpness::Result sharpnessResult;
    TIMED("runMetrics: sharpness", sharpnessResult = sharpness::gaussian(baseImage, 5));
    metrics->sharpness = sharpnessResult.sharpness;
    metrics->depthOfField = sharpness::depthOfField(sharpnessResult);
    metrics->depthOfFieldRaw = sharpnessResult.breakout;
  }

  void colorFeatures()
  {
    cv::Mat hsvImage;
    cv::cvtColor(baseImage, hsvImage, cv::COLOR_BGR2HSV);

    std::vector<cv::Mat> hsvPlanes;
    cv::split(baseImage, hsvPlanes);
    assert(hsvPlanes.size() == 3);
    hsvImage.release();

    metrics->saturation = cv::mean(hsvPlanes[1])[0];

    static_assert(metric::Hue::numberOfChannels == 6, "Wrong number of channels in Hue calculation");
    cv::Mat const hHist = iprocess::histogram(hsvPlanes[0], 6);
    hsvPlanes.clear();

    metrics->hue = metric::Hue{};
    for (std::size_t ch = 0; ch < 6; ++ch)
      metrics->hue->channel[ch] = hHist.at<float>(ch);

    cv::Mat rotatedVHist(hHist.rows, hHist.cols, hHist.type());
    hHist.rowRange(0, 3).copyTo(rotatedVHist.rowRange(4, 6));
    hHist.rowRange(3, 6).copyTo(rotatedVHist.rowRange(0, 3));
    metrics->complementary = hHist.dot(rotatedVHist);
  }

private:
  metric::MetricPtr metrics;
  cv::Mat baseImage;
};
} // unnamed namespace

metric::MetricPtr calcMetrics(cv::Mat image)
{
  Processor processor(image);
  image.release();

  processor.colorHistograms();
  processor.colorFeatures();

  processor.toGrayscale();
  processor.histogram();
  processor.blur();

  processor.equalize();
  processor.noise();
  processor.sharpness();

  return processor.result();
}

}} // namespace phobos::iprocess
