#ifndef PROCESSWIZARD_ACTION_H
#define PROCESSWIZARD_ACTION_H

#include "ProcessWizard/ActionFwd.h"
#include "PhotoContainers/ItemState.h"
#include "PhotoContainers/Fwd.h"
#include <QMetaType>
#include <QString>

namespace phobos { namespace processwiz {

class Action
{
public:
  Action(pcontainer::ItemState const matchedState);

  pcontainer::ItemState matching() const;

  virtual QString toString() const = 0;
  virtual std::size_t priority() const = 0;
  virtual bool greedy() const = 0;

  bool operator<(Action const& other) const;

protected:
  pcontainer::ItemState const matchedState;
};

class DeleteAction : public Action
{
public:
  enum class Method { Permanent, Trash };

  DeleteAction(pcontainer::ItemState const matchedState, Method const method);

  QString toString() const override;
  std::size_t priority() const override { return 0; }
  bool greedy() const override { return true; }

private:
  Method const method;
};

class RenameAction : public Action
{
public:
  RenameAction(pcontainer::ItemState const matchedState, std::string const& pattern);

  QString toString() const override;
  std::size_t priority() const override { return 1; }
  bool greedy() const override { return true; }

private:
  std::string const pattern;
};

}} // namespace phobos::processwiz

Q_DECLARE_METATYPE(phobos::processwiz::ConstActionPtr)

#endif // PROCESSWIZARD_ACTION_H