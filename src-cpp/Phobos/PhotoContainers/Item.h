#ifndef PHOTOCONTAINERS_ITEM_H
#define PHOTOCONTAINERS_ITEM_H

#include <functional>
#include <memory>
#include <vector>
#include <QObject>
#include <QUuid>
#include <QImage>
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
                   std::function<void(QImage)> onLoadCallback);

    bool isSelected() const;
    QUuid const& seriesUuid() const { return _seriesId; }
    ItemState state() const { return _state; }
    iprocess::MetricPtr metric() const { return _metric; }
    iprocess::ScoredMetricPtr scoredMetric() const { return _scoredMetric; }
    std::string const& fileName() const { return _fileName; }
    unsigned ord() const { return _ordinal; }

    void setScoredMetric(iprocess::ScoredMetricPtr const& scoredMetric);

    bool hasImage() const { return !_image.isNull(); }
    QImage const& image() const { return _image; }

public slots:
    void select();
    void discard();
    void deselect();
    void invert();
    void toggleSelection();

signals:
    void stateChanged();
    void metricsReady();

private slots:
    void loadedPhotoFromThread(QImage image);
    void metricsReadyFromThread(iprocess::MetricPtr metric);

private:
    std::string const _fileName;
    QUuid const _seriesId;
    unsigned const _ordinal;
    ItemState _state;
    QImage _image;
    iprocess::MetricPtr _metric;
    iprocess::ScoredMetricPtr _scoredMetric;
};

using ItemPtr = std::shared_ptr<Item>;
using ItemPtrVec = std::vector<ItemPtr>;

}} // namespace phobos::pcontainer

#endif // PHOTOCONTAINERS_ITEM_H
