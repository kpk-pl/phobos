#ifndef PHOTOCONTAINERS_ITEM_H
#define PHOTOCONTAINERS_ITEM_H

#include <functional>
#include <QObject>
#include <QUuid>
#include <QMetaObject>
#include "PhotoContainers/Fwd.h"
#include "PhotoContainers/ItemState.h"
#include "PhotoContainers/ItemId.h"
#include "PhotoContainers/ExifData.h"

namespace phobos { namespace pcontainer {

// TODO: Introduce UUID type signature for each Item. This will contain seriesUuid in it. Pass it
// instead of seriesUuid and filename for item

class Item : public QObject
{
    Q_OBJECT

public:
    explicit Item(QString const& fileName, QUuid const seriesId, unsigned const ordinal);

    bool isSelected() const;
    ItemState state() const { return _state; }
    unsigned ord() const { return _ordinal; }
    ExifData const& exif() const { return _exif; }

    ItemId const& id() const { return _id; }
    QUuid const& seriesUuid() const { return _id.seriesUuid; }
    QString const& fileName() const { return _id.fileName; }

signals:
    void stateChanged() const;

public slots:
    void select() const;
    void discard() const;
    void deselect() const;
    void invert() const;
    void toggleSelection() const;
    void setState(ItemState state) const;

private:
    ItemId const _id;
    unsigned const _ordinal;
    ExifData const _exif;
    mutable ItemState _state;
};

}} // namespace phobos::pcontainer

#endif // PHOTOCONTAINERS_ITEM_H
