#ifndef PHOTOCONTAINERS_ITEM_H
#define PHOTOCONTAINERS_ITEM_H

#include "PhotoContainers/Fwd.h"
#include "PhotoContainers/ItemState.h"
#include "PhotoContainers/ItemId.h"
#include "PhotoContainers/FileInfo.h"
#include "ImportWizard/Types.h"
#include <QObject>
#include <QUuid>
#include <QMetaObject>
#include <functional>

namespace phobos { namespace pcontainer {

class Item : public QObject
{
  Q_OBJECT

public:
  explicit Item(importwiz::Photo const& importedPhoto, QUuid const seriesId, unsigned const ordinal);

  bool isSelected() const;
  ItemState state() const { return _state; }
  unsigned ord() const { return _ordinal; }
  FileInfo const& info() const { return _info; }

  ItemId const& id() const { return _id; }
  QUuid const& seriesUuid() const { return _id.seriesUuid; }
  QString const& fileName() const { return _id.fileName; }

signals:
  void stateChanged() const;

public slots:
  void select() const;
  void deselect() const;
  void invert() const;
  void setState(ItemState state) const;

private:
  ItemId const _id;
  unsigned const _ordinal;
  FileInfo const _info;
  mutable ItemState _state;
};

}} // namespace phobos::pcontainer

#endif // PHOTOCONTAINERS_ITEM_H
