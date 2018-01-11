#include "PhotoContainers/Item.h"
#include <easylogging++.h>

namespace phobos { namespace pcontainer {

Item::Item(importwiz::Photo const& importedPhoto, QUuid const seriesId, unsigned const ordinal) :
  _id{seriesId, importedPhoto.name}, _ordinal(ordinal), _info(importedPhoto.info), _state(ItemState::UNKNOWN)
{}

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
    deselect();
    break;
  case ItemState::UNKNOWN:
    select();
    break;
  }
}

}} // namespace phobos::pcontainer
