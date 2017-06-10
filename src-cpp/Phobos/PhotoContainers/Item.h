#ifndef PHOTOCONTAINERS_ITEM_H
#define PHOTOCONTAINERS_ITEM_H

#include <functional>
#include <QObject>
#include <QUuid>
#include <QMetaObject>
#include "PhotoContainers/Fwd.h"

namespace phobos { namespace pcontainer {

enum class ItemState
{
    UNKNOWN,
    SELECTED,
    DISCARDED
};

// TODO: Introduce UUID type signature for each Item. This will contain seriesUuid in it. Pass it
// instead of seriesUuid and filename for item

class Item : public QObject
{
    Q_OBJECT

public:
    explicit Item(std::string const& fileName, QUuid const seriesId, unsigned const ordinal);

    bool isSelected() const;
    QUuid const& seriesUuid() const { return _seriesId; }
    ItemState state() const { return _state; }
    std::string const& fileName() const { return _fileName; }
    unsigned ord() const { return _ordinal; }

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
    std::string const _fileName;
    QUuid const _seriesId;
    unsigned const _ordinal;
    mutable ItemState _state;
};

}} // namespace phobos::pcontainer

#endif // PHOTOCONTAINERS_ITEM_H
