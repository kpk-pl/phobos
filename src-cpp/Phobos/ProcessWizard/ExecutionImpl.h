#ifndef PROCESSWIZARD_EXECUTIONIMPL_H
#define PROCESSWIZARD_EXECUTIONIMPL_H

#include "ProcessWizard/Execution.h"
#include "ProcessWizard/Action.h"

namespace phobos { namespace processwiz {

class DeleteExecution : public Execution
{
public:
  DeleteExecution(QString const& filename, DeleteAction::Method const deleteMethod);

  bool execute() const override;
  QString toString() const override;
  QString warning() const override;

private:
  QString const file;
  DeleteAction::Method const method;
};

class RenameExecution : public Execution
{
public:
  RenameExecution(QString const& file, QString const& newFilename);

  bool execute() const override;
  QString toString() const override;
  QString warning() const override;

private:
  QString const file;
  QString const newFilename;
};

}} // namespace phobos::processwiz

#endif // PROCESSWIZARD_EXECUTIONIMPL_H
