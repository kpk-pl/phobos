#ifndef PROCESSWIZARD_EXECUTION_H
#define PROCESSWIZARD_EXECUTION_H

#include "ProcessWizard/ExecutionFwd.h"
#include "PhotoContainers/ItemId.h"
#include <boost/optional.hpp>
#include <QMetaType>
#include <QString>

namespace phobos { namespace processwiz {

class Execution
{
public:
  virtual bool execute() const = 0;
  virtual QString toString() const = 0;
  virtual QString warning() const = 0;
  virtual bool destructive() const = 0;

  QString const& file() const { return _itemId.fileName; }
  pcontainer::ItemId const& itemId() const { return _itemId; }

protected:
  Execution(pcontainer::ItemId const& itemId) : _itemId(itemId)
  {}

  pcontainer::ItemId const _itemId;
};

}} // namespace phobos::executiuon

Q_DECLARE_METATYPE(phobos::processwiz::ConstExecutionPtrVec)

#endif // PROCESSWIZARD_EXECUTION_H
