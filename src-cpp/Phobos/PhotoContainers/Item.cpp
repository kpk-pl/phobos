#include <QThreadPool>
#include "PhotoContainers/Item.h"
#include "ImageProcessing/LoaderThread.h"
#include "Config.h"
#include "ConfigExtension.h"
#include "ImageProcessing/LoaderThread.h"
#include "ImageProcessing/ScalePixmap.h"

namespace phobos { namespace pcontainer {

Item::Item(std::string const& fileName, QUuid const seriesId, unsigned const ordinal) :
    _fileName(fileName), _seriesId(seriesId), _ordinal(ordinal), _state(ItemState::UNKNOWN)
{
}

namespace {
    bool sizeFits(QSize const& smaller, QSize const& bigger)
    {
        return smaller.width() <= bigger.width() && smaller.height() <= bigger.height();
    }

    QSize getPixmapSizeFromConfig()
    {
        return config::qSize("photoItem.pixmapSize").value_or(QSize(320, 240));
    }
} // unnamed namespace

void Item::loadPhoto(QSize const& size, QObject const* onLoadReceiver,
                     std::function<void(QImage)> onLoadCallback)
{
    using namespace iprocess;
    std::shared_ptr<QMetaObject::Connection> result;

    if (hasImage() && sizeFits(size, _image.size()))
        onLoadCallback(_image);
    else
    {
        bool doMetrics = !_metric;
        QSize const loadSize = getPixmapSizeFromConfig();
        LoaderThread* loaderTask = new LoaderThread(_fileName, {loadSize, size}, doMetrics);
        loaderTask->setAutoDelete(true);

        if (doMetrics)
            QObject::connect(&loaderTask->readySignals, &LoaderThreadSignals::metricsReady,
                             this, &Item::metricsReadyFromThread, Qt::QueuedConnection);

        QObject::connect(&loaderTask->readySignals, &LoaderThreadSignals::imageReady,
                         onLoadReceiver, onLoadCallback, Qt::QueuedConnection);

        if (!hasImage())
            QObject::connect(&loaderTask->readySignals, &LoaderThreadSignals::imageReady,
                             this, &Item::loadedPhotoFromThread, Qt::QueuedConnection);

        QThreadPool::globalInstance()->start(loaderTask);
    }
}

void Item::loadedPhotoFromThread(QImage image)
{
    QSize const expectedSize = getPixmapSizeFromConfig();
    if (!sizeFits(image.size(), expectedSize))
        _image = image.scaled(expectedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    else
        _image = image;
}

void Item::metricsReadyFromThread(iprocess::MetricPtr metric)
{
    _metric = metric;
    emit metricsReady();
}

void Item::setScoredMetric(iprocess::ScoredMetricPtr const& scoredMetric)
{
    _scoredMetric = scoredMetric;
    emit stateChanged();
}

bool Item::isSelected() const
{
    return _state == ItemState::SELECTED;
}

void Item::select() const
{
    _state = ItemState::SELECTED;
    emit stateChanged();
}

void Item::discard() const
{
    _state = ItemState::DISCARDED;
    emit stateChanged();
}

void Item::deselect() const
{
    _state = ItemState::UNKNOWN;
    emit stateChanged();
}

void Item::setState(ItemState state) const
{
    _state = state;
    emit stateChanged();
}

void Item::invert() const
{
    switch(_state)
    {
    case ItemState::SELECTED:
        discard();
        break;
    case ItemState::DISCARDED:
        select();
        break;
    default:
        break;
    }
}

void Item::toggleSelection() const
{
    switch(_state)
    {
    case ItemState::UNKNOWN:
        select();
        break;
    case ItemState::SELECTED:
        discard();
        break;
    case ItemState::DISCARDED:
        deselect();
        break;
    }
}

}} // namespace phobos::pcontainer
