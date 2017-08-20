#ifndef PROCESSWIZARD_EXECUTIONIMPL_H
#define PROCESSWIZARD_EXECUTIONIMPL_H

#include "ProcessWizard/Execution.h"

namespace phobos { namespace processwiz {

class DeleteExecution : public Execution
{
public:
  DeleteExecution(QString const& filename);

  void execute() const override;
  QString toString() const override;
  QString warning() const override;

private:
  QString const file;
};

class RenameExecution : public Execution
{
public:
  RenameExecution(QString const& file, std::string const& newFilename);

  void execute() const override;
  QString toString() const override;
  QString warning() const override;

private:
  QString const file;
  std::string const newFilename;
};

}} // namespace phobos::processwiz

#endif // PROCESSWIZARD_EXECUTIONIMPL_H
