#include "ProcessWizard/ExecutionImpl.h"

namespace phobos { namespace processwiz {

DeleteExecution::DeleteExecution(QString const& file) :
  file(file)
{}

void DeleteExecution::execute() const
{
}

QString DeleteExecution::toString() const
{
  return QString("Delete \"%1\" permanently").arg(file);
}

QString DeleteExecution::warning() const
{
  return QString();
}

RenameExecution::RenameExecution(QString const& file, std::string const& newFilename) :
  file(file), newFilename(newFilename)
{}

void RenameExecution::execute() const
{
}

QString RenameExecution::toString() const
{
  return "Rename";
}

QString RenameExecution::warning() const
{
  return QString();
}

}} // namespace phobos::processwiz
