#ifndef IMAGE_PROCESSING_LOADERTHREAD_H
#define IMAGE_PROCESSING_LOADERTHREAD_H

#include <string>
#include <vector>
#include <memory>
#include <opencv2/core/core.hpp>
#include <QRunnable>
#include <QObject>
#include <QSize>
#include <QPixmap>
#include "ImageProcessing/Metrics.h"

namespace phobos { namespace iprocess {

class LoaderThreadSignals : public QObject
{
    Q_OBJECT
signals:
    void pixmapReady(std::shared_ptr<QPixmap>) const;
    void metricsReady(phobos::iprocess::MetricPtr) const;
};

class LoaderThread : public QRunnable
{
public:
    explicit LoaderThread(std::string const& fileName,
                          std::vector<QSize> const& requestedSizes,
                          bool calculateMetrics = false);

    void run() override;

    LoaderThreadSignals readySignals;

private:
    void emitLoadedSignal(QPixmap pixmap);
    void emitLoadedSignal(cv::Mat const& cvImage);
    void runMetrics(cv::Mat cvImage) const;
    QSize biggestClosestSize(QSize const& pixmapSize) const;

    std::string const fileToLoad;
    std::vector<QSize> const requestedSizes;
    bool const calculateMetrics;
};

}} // namespace phobos::iprocess

#endif // IMAGE_PROCESSING_LOADERTHREAD_H
