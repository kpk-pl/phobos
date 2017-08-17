#include "ProcessWizard/Action.h"
#include "Utils/LexicalCast.h"
#include <QObject>
#include <cassert>

namespace phobos { namespace processwiz {

Action::Action(pcontainer::ItemState const matchedState) :
  matchedState(matchedState)
{
}

pcontainer::ItemState Action::matching() const
{
  return matchedState;
}

bool Action::operator<(Action const& other) const
{
  return priority() < other.priority();
}

DeleteAction::DeleteAction(pcontainer::ItemState const matchedState, Method const method) :
  Action(matchedState), method(method)
{
}

QString DeleteAction::toString() const
{
  switch(method) {
  case Method::Permanent:
    return QObject::tr("Delete %1 photos permanently")
                    .arg(QString::fromStdString(utils::lexicalCast(matchedState)));
  case Method::Trash:
    return QObject::tr("Move %1 photos to trash")
                    .arg(QString::fromStdString(utils::lexicalCast(matchedState)));
  }

  assert(false);
  return QString();
}

RenameAction::RenameAction(const pcontainer::ItemState matchedState) :
  Action(matchedState)
{
}

QString RenameAction::toString() const
{
  return QObject::tr("Rename");
}

}} // namespace phobos::processwiz
