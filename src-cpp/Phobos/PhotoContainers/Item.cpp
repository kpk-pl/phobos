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
                     std::function<void(std::shared_ptr<QPixmap> const&)> onLoadCallback)
{
    using namespace iprocess;
    std::shared_ptr<QMetaObject::Connection> result;

    if (hasPixmap() && sizeFits(size, _pixmap->size()))
        onLoadCallback(_pixmap);
    else
    {
        bool doMetrics = !_metric;
        QSize const loadSize = getPixmapSizeFromConfig();
        LoaderThread* loaderTask = new LoaderThread(_fileName, {loadSize, size}, doMetrics);
        loaderTask->setAutoDelete(true);

        if (doMetrics)
            QObject::connect(&loaderTask->readySignals, &LoaderThreadSignals::metricsReady,
                             this, &Item::metricsReadyFromThread, Qt::QueuedConnection);

        QObject::connect(&loaderTask->readySignals, &LoaderThreadSignals::pixmapReady,
                         onLoadReceiver, onLoadCallback, Qt::QueuedConnection);

        if (!_pixmap)
            QObject::connect(&loaderTask->readySignals, &LoaderThreadSignals::pixmapReady,
                             this, &Item::loadedPhotoFromThread, Qt::QueuedConnection);

        QThreadPool::globalInstance()->start(loaderTask);
    }
}

void Item::loadedPhotoFromThread(std::shared_ptr<QPixmap> pixmap)
{
    QSize const expectedSize = getPixmapSizeFromConfig();
    if (!sizeFits(pixmap->size(), expectedSize))
        _pixmap = std::make_shared<QPixmap>(iprocess::scalePixmap(*pixmap, expectedSize));
    else
        _pixmap = pixmap;
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

void Item::select()
{
    _state = ItemState::SELECTED;
    emit stateChanged();
}

void Item::discard()
{
    _state = ItemState::DISCARDED;
    emit stateChanged();
}

void Item::invert()
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

void Item::toggleSelection()
{
    switch(_state)
    {
    case ItemState::UNKNOWN:
        _state = ItemState::SELECTED;
        break;
    case ItemState::SELECTED:
        _state = ItemState::DISCARDED;
        break;
    case ItemState::DISCARDED:
        _state = ItemState::UNKNOWN;
        break;
    }
    emit stateChanged();
}

}} // namespace phobos::pcontainer
