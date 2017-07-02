#ifndef IMAGE_PROCESSING_LOADERTHREAD_H
#define IMAGE_PROCESSING_LOADERTHREAD_H

#include <string>
#include <vector>
#include <memory>
#include <opencv2/core/core.hpp>
#include <QRunnable>
#include <QObject>
#include <QSize>
#include <QImage>
#include "ImageProcessing/Metrics.h"
#include "PhotoContainers/ItemId.h"

namespace phobos { namespace iprocess {

class LoaderThreadSignals : public QObject
{
    Q_OBJECT

signals:
    void imageReady(pcontainer::ItemId, QImage) const;
    void metricsReady(pcontainer::ItemId, phobos::iprocess::MetricPtr) const;
};

class LoaderThread : public QRunnable
{
public:
    explicit LoaderThread(pcontainer::ItemId const& itemId, QSize const& requestedSize);

    void withMetrics(bool calculate) { calculateMetrics = calculate; }
    void run() override;

    LoaderThreadSignals readySignals;

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
