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

RenameExecution::RenameExecution(pcontainer::ItemId const& itemId, QString const& newFilename) :
  Execution(itemId), newFilename(newFilename)
{}

bool RenameExecution::execute() const
{
  bool const debugDisabled = config::qualified("debug.disableExecutionOperations", false);

  LOG(INFO) << "Renaming " << itemId().fileName << " to " << newFilename;
  return debugDisabled || QFile::rename(itemId().fileName, newFilename);
}

QString RenameExecution::toString() const
{
  return QString("Rename \"%1\" to \"%2\"").arg(file(), newFilename);
}

CopyExecution::CopyExecution(pcontainer::ItemId const& itemId,
                             QString const& destFilename) :
  Execution(itemId), destFilename(destFilename)
{}

bool CopyExecution::execute() const
{
  bool const debugDisabled = config::qualified("debug.disableExecutionOperations", false);

  LOG(INFO) << "Copying " << itemId().fileName << " to " << destFilename;
  return debugDisabled || QFile::copy(itemId().fileName, destFilename);
}

QString CopyExecution::toString() const
{
  return QString("Copy \"%1\" to \"%2\"").arg(file(), destFilename);
}

}} // namespace phobos::processwiz
