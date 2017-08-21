#include "ProcessWizard/ExecutionImpl.h"
#include "Config.h"
#include "ConfigExtension.h"
#include "Utils/Filesystem/Trash.h"
#include <easylogging++.h>
#include <QFile>
#include <cassert>

namespace phobos { namespace processwiz {

DeleteExecution::DeleteExecution(QString const& file, DeleteAction::Method const deleteMethod) :
  file(file), method(deleteMethod)
{}

void DeleteExecution::execute() const
{
  bool const debugDisabled = config::qualified("debug.disableExecutionOperations", false);

  switch(method)
  {
  case DeleteAction::Method::Permanent:
    LOG(INFO) << "Removing permanently \"" << file << '"';
    if (!debugDisabled)
      QFile(file).remove();
    break;
  case DeleteAction::Method::Trash:
    LOG(INFO) << "Removing to trash \"" << file << '"';
    if (!debugDisabled)
      utils::fs::moveToTrash(file);
    break;
  }
}

QString DeleteExecution::toString() const
{
  switch(method)
  {
  case DeleteAction::Method::Permanent:
    return QString("Delete \"%1\" permanently").arg(file);
  case DeleteAction::Method::Trash:
    return QString("Move \"%1\" to trash").arg(file);
  }

  assert(false);
  return QString{};
}

QString DeleteExecution::warning() const
{
  return QString();
}

RenameExecution::RenameExecution(QString const& file, QString const& newFilename) :
  file(file), newFilename(newFilename)
{}

void RenameExecution::execute() const
{
}

QString RenameExecution::toString() const
{
  return QString("Rename \"%1\" to \"%2\"").arg(file, newFilename);
}

QString RenameExecution::warning() const
{
  return QString();
}

}} // namespace phobos::processwiz
