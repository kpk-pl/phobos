#include "ProcessWizard/Execution/Execute.h"
#include "ProcessWizard/Execution/Execution.h"
#include "PhotoContainers/Set.h"
#include "Utils/Comparators.h"
#include <easylogging++.h>
#include <QMessageBox>
#include <QString>
#include <QApplication>
#include <QStyle>

namespace phobos { namespace processwiz { namespace exec {

namespace {
void displayFailures(ConstExecutionPtrVec const& failures)
{
  QString message;
  for (auto const& failure : failures)
    message += failure->toString() + "\n";

  QMessageBox mBox(QMessageBox::Critical, "Some operations failed", message, QMessageBox::Ok);

  LOG(TRACE) << "Displaying dialog showing " <<  failures.size() << " failed operations";
  mBox.exec();
}
}

void execute(pcontainer::Set &photoSet, ConstExecutionPtrVec const& executions)
{
  ConstExecutionPtrVec failed;
  std::vector<pcontainer::ItemId> destroyed;

  LOG(TRACE) << "Executing " << executions.size() << " operations";
  for (auto const& exec : executions)
  {
    if (!exec->execute())
    {
      failed.push_back(exec);
      LOG(WARNING) << "Failed: " << exec->toString();
    }
    else if (exec->destructive())
    {
      destroyed.push_back(exec->itemId());
    }
  }

  if (!failed.empty())
    displayFailures(failed);

  photoSet.removeImages(destroyed);
}

}}} // namespace phobos::processwiz::exec
