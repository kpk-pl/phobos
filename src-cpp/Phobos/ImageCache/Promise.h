#ifndef PHOBOS_IMAGECACHE_PROMISE_H_
#define PHOBOS_IMAGECACHE_PROMISE_H_

#include <QObject>
#include <QImage>
#include "ImageCache/PromiseFwd.h"
#include "ImageProcessing/Metrics.h"

namespace phobos { namespace icache {

class Promise : public QObject
{
    Q_OBJECT

    class CannotStartError : public std::runtime_error
    {
    public:
        CannotStartError() : std::runtime_error("Cannot start promise")
        {}
    };

public:
    explicit Promise(QImage const& readyImage);
    explicit Promise(std::string const& imageFilename);

    static PromisePtr create(QImage const& readyImage);
    static PromisePtr create(std::string const& filenameToLoad);

    void enableMetricsCall();
    void setThreadPriority(int const priority);
    void start() const;

    operator bool() const;
    QImage const& getImage() const;

signals:
    void metricsReady(phobos::iprocess::MetricPtr);
    void imageReady(QImage);

private slots:
    void imageReadyFromThread(QImage image);

private:
    QImage mutable readyImage;
    boost::optional<std::string> const imageFilename;
    bool callMetrics;
    int threadPriority;
};

}} // namespace phobos::icache

#endif // PHOBOS_IMAGECACHE_PROMISE_H_
