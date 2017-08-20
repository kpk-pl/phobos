#include "ProcessWizard/Action.h"
#include "ProcessWizard/ExecutionImpl.h"
#include "PhotoContainers/Set.h"
#include "PhotoContainers/Series.h"
#include "PhotoContainers/Item.h"
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

ConstExecutionPtrVecConstPtr
  DeleteAction::makeExecutions(pcontainer::Set const& photoSet,
                               SeriesCounts const&) const
{
  auto result = std::make_shared<ConstExecutionPtrVec>();

  // TODO: Handle trash remove
  for (pcontainer::SeriesPtr const& series : photoSet)
    for (pcontainer::ItemPtr const& photo : *series)
      if (photo->state() == matchedState)
        result->push_back(std::make_shared<DeleteExecution>(photo->fileName()));

  return result;
}

RenameAction::RenameAction(const pcontainer::ItemState matchedState, std::string const& pattern) :
  Action(matchedState), pattern(pattern)
{
}

QString RenameAction::toString() const
{
  return QObject::tr("Rename each %1 photo to \"%2\"")
      .arg(QString::fromStdString(utils::lexicalCast(matchedState)))
      .arg(QString::fromStdString(pattern));
}

ConstExecutionPtrVecConstPtr
  RenameAction::makeExecutions(pcontainer::Set const& photoSet,
                               SeriesCounts const&) const
{
  auto result = std::make_shared<ConstExecutionPtrVec>();

  for (pcontainer::SeriesPtr const& series : photoSet)
    for (pcontainer::ItemPtr const& photo : *series)
      if (photo->state() == matchedState)
        result->push_back(std::make_shared<RenameExecution>(photo->fileName(), "TODO"));

  return result;
}

}} // namespace phobos::processwiz
