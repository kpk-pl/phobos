#ifndef IMAGE_PROCESSING_LOADERTHREAD_H
#define IMAGE_PROCESSING_LOADERTHREAD_H

#include "ImageProcessing/Metrics.h"
#include "PhotoContainers/ItemId.h"
#include "ImageCache/Runnable.h"
#include <opencv2/core/core.hpp>
#include <QObject>
#include <QSize>
#include <QImage>

namespace phobos { namespace iprocess {

class LoaderThreadSignals : public QObject
{
  Q_OBJECT

signals:
  void imageReady(pcontainer::ItemId, QImage) const;
  void metricsReady(pcontainer::ItemId, phobos::iprocess::MetricPtr) const;
};

class NullImageException : public std::runtime_error
{
public:
  NullImageException(std::string const& filename) :
    std::runtime_error("Cannot load image: " + filename)
  {}
};

class LoaderThread : public icache::Runnable
{
public:
  explicit LoaderThread(pcontainer::ItemId const& itemId, QSize const& requestedSize);

  void withMetrics(bool calculate) { calculateMetrics = calculate; }
  icache::Runnable::Id id() const override;

  LoaderThreadSignals readySignals;

protected:
  void runImpl() override;

private:
  void runWithoutMetrics() const;
  void emitLoadedSignal(cv::Mat const& cvImage);
  void runMetrics(cv::Mat cvImage) const;

  pcontainer::ItemId const itemId;
  QSize const requestedSize;
  bool calculateMetrics;
};

}} // namespace phobos::iprocess

#endif // IMAGE_PROCESSING_LOADERTHREAD_H
