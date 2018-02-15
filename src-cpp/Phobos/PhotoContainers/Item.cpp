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

void Item::reset() const
{
  LOG(INFO) << "Reset item " << _id.toString();
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
    reset();
    break;
  }
}

void Item::invert() const
{
  switch(_state)
  {
  case ItemState::SELECTED:
    reset();
    break;
  case ItemState::UNKNOWN:
    select();
    break;
  }
}

}} // namespace phobos::pcontainer
