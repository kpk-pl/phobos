#ifndef PROCESSWIZARD_EXECUTIONIMPL_H
#define PROCESSWIZARD_EXECUTIONIMPL_H

#include "ProcessWizard/Execution/Execution.h"
#include "ProcessWizard/Action.h"
#include "PhotoContainers/ItemId.h"

namespace phobos { namespace processwiz {

class DeleteExecution : public Execution
{
public:
  DeleteExecution(pcontainer::ItemId const& itemId, DeleteAction::Method const deleteMethod);

  bool execute() const override;
  QString toString() const override;
  bool destructive() const override { return true; }

private:
  DeleteAction::Method const method;
};

class RenameExecution : public Execution
{
public:
  RenameExecution(pcontainer::ItemId const& itemId, QString const& newFilename);

  bool execute() const override;
  QString toString() const override;
  bool destructive() const override { return true; }

private:
  QString const newFilename;
};

class CopyExecution : public Execution
{
public:
  CopyExecution(pcontainer::ItemId const& itemId, QString const& destFilename);

  bool execute() const override;
  QString toString() const override;
  bool destructive() const override { return false; }

private:
  QString const destFilename;
};

}} // namespace phobos::processwiz

#endif // PROCESSWIZARD_EXECUTIONIMPL_H
