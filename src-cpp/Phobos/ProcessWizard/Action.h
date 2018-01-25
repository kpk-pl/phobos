#ifndef PROCESSWIZARD_ACTION_H
#define PROCESSWIZARD_ACTION_H

#include "ProcessWizard/ActionFwd.h"
#include "ProcessWizard/Operation.h"
#include "ProcessWizard/Execution/ExecutionFwd.h"
#include "PhotoContainers/ItemState.h"
#include "PhotoContainers/Fwd.h"
#include <QMetaType>
#include <QString>
#include <QDir>

namespace phobos { namespace processwiz {

struct SeriesCounts;

class Action
{
public:
  Action(pcontainer::ItemState const matchedState);

  pcontainer::ItemState matching() const;

  virtual OperationType operation() const = 0;
  virtual QString toString() const = 0;
  virtual std::size_t priority() const = 0;
  virtual bool greedy() const = 0;

  virtual ConstExecutionPtrVec makeExecutions(pcontainer::Set const& photoSet,
                                              SeriesCounts const& counts) const = 0;

  bool operator<(Action const& other) const;

protected:
  pcontainer::ItemState const matchedState;
};

// TODO: Delete action to trash takes a lot of time.
// Prifile if this is a pure winapi call time or something else
// Maybe winapi allows deleting more than one item at once?
class DeleteAction : public Action
{
public:
  enum class Method { Permanent, Trash };

  DeleteAction(pcontainer::ItemState const matchedState, Method const method);

  OperationType operation() const override { return OperationType::Delete; }
  QString toString() const override;
  std::size_t priority() const override { return 3; }
  bool greedy() const override { return true; }

  ConstExecutionPtrVec makeExecutions(pcontainer::Set const& photoSet,
                                      SeriesCounts const& counts) const override;

private:
  Method const method;
};

class RenameAction : public Action
{
public:
  RenameAction(pcontainer::ItemState const matchedState, QString const& pattern);

  OperationType operation() const override { return OperationType::Rename; }
  QString toString() const override;
  std::size_t priority() const override { return 2; }
  bool greedy() const override { return true; }

  ConstExecutionPtrVec makeExecutions(pcontainer::Set const& photoSet,
                                      SeriesCounts const& counts) const override;

private:
  QString const pattern;
};

class MoveAction : public Action
{
public:
  MoveAction(pcontainer::ItemState const matchedState, QDir const& destination, QString const& optRenamePattern);

  OperationType operation() const override { return OperationType::Move; }
  QString toString() const override;
  std::size_t priority() const override { return 1; }
  bool greedy() const override { return true; }

  ConstExecutionPtrVec makeExecutions(pcontainer::Set const& photoSet,
                                      SeriesCounts const& counts) const override;

private:
  QDir const destination;
  QString const optPattern;
};

class CopyAction : public Action
{
public:
  CopyAction(pcontainer::ItemState const matchedState, QDir const& destination, QString const& optRenamePattern);

  OperationType operation() const override { return OperationType::Copy; }
  QString toString() const override;
  std::size_t priority() const override { return 0; }
  bool greedy() const override { return false; }

  ConstExecutionPtrVec makeExecutions(pcontainer::Set const& photoSet,
                                      SeriesCounts const& counts) const override;

private:
  QDir const destination;
  QString const optPattern;
};

}} // namespace phobos::processwiz

Q_DECLARE_METATYPE(phobos::processwiz::ConstActionPtr)
Q_DECLARE_METATYPE(phobos::processwiz::ConstActionPtrVec)

#endif // PROCESSWIZARD_ACTION_H
