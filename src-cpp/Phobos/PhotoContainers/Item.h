#ifndef PHOTOCONTAINERS_ITEM_H
#define PHOTOCONTAINERS_ITEM_H

#include <functional>
#include <memory>
#include <vector>
#include <QObject>
#include <QUuid>
#include <QPixmap>
#include <QMetaObject>
#include "ImageProcessing/Metrics.h"

namespace phobos { namespace pcontainer {

enum class ItemState
{
    UNKNOWN,
    SELECTED,
    DISCARDED
};

class Item : public QObject
{
    Q_OBJECT

public:
    explicit Item(std::string const& fileName, QUuid const seriesId, unsigned const ordinal);

    /*
     * Loads new pixmap in separate thread.
     * Receiver is notified with a signal.
     */
    void loadPhoto(QSize const& size, QObject const* onLoadReceiver,
                   std::function<void(std::shared_ptr<QPixmap> const&)> onLoadCallback);

    bool isSelected() const;
    QUuid const& seriesUuid() const { return _seriesId; }
    ItemState state() const { return _state; }
    iprocess::MetricPtr metric() const { return _metric; }
    iprocess::ScoredMetricPtr scoredMetric() const { return _scoredMetric; }
    std::string const& fileName() const { return _fileName; }
    unsigned ord() const { return _ordinal; }

    void setScoredMetric(iprocess::ScoredMetricPtr const& scoredMetric);

    bool hasPixmap() const { return _pixmap && !_pixmap->isNull(); }
    std::shared_ptr<QPixmap> const& pixmap() const { return _pixmap; }

public slots:
    void select();
    void discard();
    void toggleSelection();

signals:
    void stateChanged();
    void metricsReady();

private slots:
    void loadedPhotoFromThread(std::shared_ptr<QPixmap> pixmap);
    void metricsReadyFromThread(iprocess::MetricPtr metric);

private:
    std::string const _fileName;
    QUuid const _seriesId;
    unsigned const _ordinal;
    ItemState _state;
    std::shared_ptr<QPixmap> _pixmap;
    iprocess::MetricPtr _metric;
    iprocess::ScoredMetricPtr _scoredMetric;
};

using ItemPtr = std::shared_ptr<Item>;
using ItemPtrVec = std::vector<ItemPtr>;

}} // namespace phobos::pcontainer

#endif // PHOTOCONTAINERS_ITEM_H
