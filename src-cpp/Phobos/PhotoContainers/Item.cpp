#include "PhotoContainers/Item.h"

namespace phobos { namespace pcontainer {

Item::Item(std::string const& fileName, QUuid const seriesId, unsigned const ordinal) :
    _id{seriesId, QString::fromStdString(fileName)},
    _ordinal(ordinal), _state(ItemState::UNKNOWN)
{
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
