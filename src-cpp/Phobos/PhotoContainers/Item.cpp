#include "PhotoContainers/Item.h"
#include <easylogging++.h>

namespace phobos { namespace pcontainer {

Item::Item(importwiz::Photo const& importedPhoto,
           QUuid const seriesId,
           unsigned const ordinal,
           bool const isFromASeries) :
  isFromASeries(isFromASeries),
  _id{seriesId, importedPhoto.name},
  _ordinal(ordinal),
  _info(importedPhoto.info),
  _state(ItemState::IGNORED)
{}

bool Item::isSelected() const
{
  return _state == ItemState::SELECTED;
}

void Item::select() const
{
  LOG(TRACE) << "Selected item " << _id.toString();
  _state = ItemState::SELECTED;
  emit stateChanged();
}

void Item::ignore() const
{
  LOG(TRACE) << "Reset item " << _id.toString();
  _state = ItemState::IGNORED;
  emit stateChanged();
}

void Item::setState(ItemState state) const
{
  switch(state)
  {
  case ItemState::SELECTED:
    select();
    break;
  case ItemState::IGNORED:
    ignore();
    break;
  }
}

void Item::invert() const
{
  switch(_state)
  {
  case ItemState::SELECTED:
    ignore();
    break;
  case ItemState::IGNORED:
    select();
    break;
  }
}

}} // namespace phobos::pcontainer
