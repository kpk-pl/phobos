#include "PhotoContainers/Item.h"
#include <easylogging++.h>

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
  LOG(INFO) << "Selected item " << _id.toString();
  _state = ItemState::SELECTED;
  emit stateChanged();
}

void Item::discard() const
{
  LOG(INFO) << "Discarded item " << _id.toString();
  _state = ItemState::DISCARDED;
  emit stateChanged();
}

// TODO: Change to "reset"
void Item::deselect() const
{
  LOG(INFO) << "Deselected item " << _id.toString();
  _state = ItemState::UNKNOWN;
  emit stateChanged();
}

void Item::setState(ItemState state) const
{
  switch(state)
  {
  case ItemState::SELECTED:
    select();
    break;
  case ItemState::DISCARDED:
    discard();
    break;
  case ItemState::UNKNOWN:
    deselect();
    break;
  }
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
