#include "ImageProcessing/MetricCalculator.h"
#include "ImageProcessing/Metric/All.h"
#include "ImageProcessing/Calculator/All.h"
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

cv::Mat prepareScaled(cv::Mat image)
{
  QSize const maxSize = config::qSize(configPath("processingSize"), QSize(1920, 1080));
  QSize const scaledSize = QSize(image.cols, image.rows).scaled(maxSize, Qt::KeepAspectRatio);
  LOG(DEBUG) << "Scaling image for metrics from " << image.cols << "x" << image.rows
             << " to " << scaledSize.width() << "x" << scaledSize.height();

  cv::Mat result;
  cv::resize(image, result, cv::Size(scaledSize.width(), scaledSize.height()), 0, 0, cv::INTER_CUBIC);
  return result;
}

class Processor
{
public:
  Processor(cv::Mat image) :
    metrics(std::make_shared<Metric>()),
    baseImage(prepareScaled(std::move(image)))
  {}

  MetricPtr result() const
  {
    return metrics;
  }

  bool isGrayscale() const
  {
    return baseImage.channels() == 1;
  }

  void toGrayscale()
  {
    TIMED("runMetrics: toGrayscale", cv::cvtColor(baseImage, baseImage, cv::COLOR_BGR2GRAY));
  }

  void equalize()
  {
    assert(baseImage.channels() == 1);
    cv::Mat result;
    TIMED("runMetrics: equalize", cv::equalizeHist(baseImage, result));
    baseImage = result;
  }

  void colorHistograms()
  {
    if (metrics->saturation.isGrayscale())
      return;

    std::vector<cv::Mat> bgrPlanes;
    TIMED("runMetrics: splitHist", cv::split(baseImage, bgrPlanes));
    assert(bgrPlanes.size() == 3);

    using Channel = feature::Histogram::Channel;
    TIMED("runMetrics: blueHistogram", metrics->histogram.data.emplace(Channel::Blue, calc::normalizedHistogram(bgrPlanes[0], nullptr)));
    TIMED("runMetrics: greenHistogram", metrics->histogram.data.emplace(Channel::Green, calc::normalizedHistogram(bgrPlanes[1], nullptr)));
    TIMED("runMetrics: redHistogram", metrics->histogram.data.emplace(Channel::Red, calc::normalizedHistogram(bgrPlanes[2], nullptr)));
  }

  void hsvColorFeatures()
  {
    cv::Mat hsvImage;
    TIMED("runMetrics: toHSV", cv::cvtColor(baseImage, hsvImage, cv::COLOR_BGR2HSV));
    TIMED("runMetrics: saturation", metrics->saturation = calc::saturation(hsvImage));

    if (!metrics->saturation.isGrayscale())
    {
      TIMED("runMetrics: hueChannels", metrics->hue = calc::hueChannels(hsvImage));
      TIMED("runMetrics: hueCompl", metrics->complementary = calc::complementaryChannels(metrics->hue.get()));
    }
  }

  void histogramAndContrast()
  {
    using Channel = feature::Histogram::Channel;
    double contrast;
    TIMED("runMetrics: contrast", metrics->histogram.data.emplace(Channel::Value, calc::normalizedHistogram(baseImage, &contrast)));

    metrics->contrast = metric::Contrast(contrast);
  }

  void noise()
  {
    TIMED("runMetrics: noise", metrics->noise = calc::noiseMeasure(baseImage, config::qualified(configPath("noiseMedianSize"), 3)));
  }

  void blur()
  {
    std::string const blurAlgo = config::qualified(configPath("blurAlgorithm"), std::string("laplace"));
    std::size_t const blurROISide = config::qualified<std::size_t>(configPath("blurRoiSide"), 1);

    if (blurAlgo == "sobel")
      TIMED("runMetrics: sobel", metrics->blur = calc::Bluriness<calc::blur::Sobel>{blurROISide}(baseImage));
    else if (blurAlgo == "laplaceMod")
      TIMED("runMetrics: laplaceMod", metrics->blur = calc::Bluriness<calc::blur::LaplaceMod>{blurROISide}(baseImage));
    else
      TIMED("runMetrics: laplace", metrics->blur = calc::Bluriness<calc::blur::Laplace>{blurROISide}(baseImage));
  }

  void sharpness()
  {
    TIMED("runMetrics: sharpness", {
      calc::Sharpness<calc::sharpness::Gaussian> sharpCalc(baseImage, 5);
      metrics->sharpness = sharpCalc.sharpness();
      metrics->depthOfField = sharpCalc.depthOfField();
    });
  }

private:
  MetricPtr metrics;
  cv::Mat baseImage;
};
} // unnamed namespace

MetricPtr calcMetrics(cv::Mat image)
{
  Processor processor(std::move(image));
  image.release();

  if (!processor.isGrayscale())
  {
    processor.hsvColorFeatures();
    processor.colorHistograms();
    processor.toGrayscale();
  }

  processor.histogramAndContrast();
  processor.blur();

  processor.equalize();
  processor.noise();
  processor.sharpness();

  return processor.result();
}

}} // namespace phobos::iprocess
