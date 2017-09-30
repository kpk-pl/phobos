#include "ProcessWizard/Execution/ExecutionImpl.h"
#include "Config.h"
#include "ConfigExtension.h"
#include "Utils/Filesystem/Trash.h"
#include "Utils/Asserted.h"
#include <easylogging++.h>
#include <QFile>
#include <cassert>

namespace phobos { namespace processwiz {

DeleteExecution::DeleteExecution(pcontainer::ItemId const& itemId, DeleteAction::Method const deleteMethod) :
  Execution(itemId), method(deleteMethod)
{}

bool DeleteExecution::execute() const
{
  bool const debugDisabled = config::qualified("debug.disableExecutionOperations", false);

  switch(method)
  {
  case DeleteAction::Method::Permanent:
    LOG(INFO) << "Removing permanently \"" << file() << '"';
    return debugDisabled || QFile(file()).remove();
  case DeleteAction::Method::Trash:
    LOG(INFO) << "Removing to trash \"" << file() << '"';
    return debugDisabled || utils::fs::moveToTrash(file());
  }

  return utils::asserted::always;
}

QString DeleteExecution::toString() const
{
  switch(method)
  {
  case DeleteAction::Method::Permanent:
    return QString("Delete \"%1\" permanently").arg(file());
  case DeleteAction::Method::Trash:
    return QString("Move \"%1\" to trash").arg(file());
  }

  return utils::asserted::always;
}

QString DeleteExecution::warning() const
{
  return QString();
}

RenameExecution::RenameExecution(pcontainer::ItemId const& itemId, QString const& newFilename) :
  Execution(itemId), newFilename(newFilename)
{}

bool RenameExecution::execute() const
{
  return false;
}

QString RenameExecution::toString() const
{
  return QString("Rename \"%1\" to \"%2\"").arg(file(), newFilename);
}

QString RenameExecution::warning() const
{
  return QString();
}

MoveExecution::MoveExecution(pcontainer::ItemId const& itemId,
                             QDir const& destination,
                             QString const& newFilename) :
  Execution(itemId), destination(destination), newFilename(newFilename)
{}

bool MoveExecution::execute() const
{
  return false;
}

QString MoveExecution::toString() const
{
  return QString("Move \"%1\" to \"%2\"").arg(file(), destination.filePath(newFilename));
}

QString MoveExecution::warning() const
{
  return QString{};
}

CopyExecution::CopyExecution(pcontainer::ItemId const& itemId,
                             QDir const& destination,
                             QString const& newFilename) :
  Execution(itemId), destination(destination), newFilename(newFilename)
{}

bool CopyExecution::execute() const
{
  return false;
}

QString CopyExecution::toString() const
{
  return QString("Copy \"%1\" to \"%2\"").arg(file(), destination.filePath(newFilename));
}

QString CopyExecution::warning() const
{
  return QString{};
}

}} // namespace phobos::processwiz
