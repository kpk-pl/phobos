#include "ProcessWizard/SummaryPage.h"
#include "ProcessWizard/Action.h"
#include "ProcessWizard/OperationIcon.h"
#include "ProcessWizard/Execution/Execution.h"
#include "Utils/Asserted.h"
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QStringList>
#include <QString>
#include <QLabel>
#include <algorithm>
#include <easylogging++.h>

namespace phobos { namespace processwiz {

// TODO: It is possible that %F flag will not be unique in result (when source images are from different directories)
// 1. Implement action simulation. Eack execution should return input and output file path.
//    Results from all execution simulations should be aggregated and displayed on summary page (or create a page after summary page
//    that will be displayed only in case of any errors.
//    Enable user to select conflict resolutions
//    a) ignore and skip failing actions
//    b) append something to filepath (like _n suffix)
// 2. %F flag should stop be handled as a unique resolution
// 3. When some executions fail, files are removed from application. Trying to execute similar actions will mess up numbering of output files
//    figure out a way to use ord() numbers and use absolute number of photos in series (photos existing + removed) to fix up numbering
//
//  TODO: If displaying sumary page without any action created, display label with warning
//  saying that user needs to "Create action" on previous page

SummaryPage::SummaryPage(SeriesCounts const& seriesCounts, pcontainer::Set const& photoSet) :
  seriesCounts(seriesCounts), photoSet(photoSet)
{
  LOG(DEBUG) << "Creating page";

  setTitle(tr("Summary"));
  setButtonText(QWizard::WizardButton::FinishButton, tr("Execute"));

  actionTree = new QTreeWidget();
  actionTree->header()->close();
  actionTree->setColumnCount(2);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(new QLabel(tr("Planned actions:")));
  layout->addWidget(actionTree);
  setLayout(layout);

  registerField("executions", this, "executions", SIGNAL(executionsChanged(ConstExecutionPtrVec)));
}

void SummaryPage::updateExecutioners(ConstActionPtrVec const& currentActions)
{
  for (auto it = actionExecs.begin(); it != actionExecs.end(); )
  {
    if (std::find(currentActions.begin(), currentActions.end(), it->first) != currentActions.end())
      ++it;
    else
      it = actionExecs.erase(it);
  }

  if (actionExecs.size() == currentActions.size())
    return;

  for (auto const& action : currentActions)
    if (actionExecs.find(action) == actionExecs.end())
      actionExecs.emplace(action, action->makeExecutions(photoSet, seriesCounts));

  assert(actionExecs.size() == currentActions.size());
}

void SummaryPage::initializePage()
{
  LOG(INFO) << "Initializing summary page";

  auto const& selectedActions = field("chosenActions").value<ConstActionPtrVec>();
  LOG(INFO) << "Displaying summary for " << selectedActions.size() << " selected actions";

  updateExecutioners(selectedActions);

  for (auto const& action : actionExecs)
  {
    auto const actionStr = action.first->toString();
    LOG(INFO) << "Enabled: " << actionStr;

    QTreeWidgetItem *topItem = new QTreeWidgetItem(actionTree);
    topItem->setIcon(0, operationIcon(action.first->operation()));
    topItem->setText(1, actionStr);
    topItem->setFlags(topItem->flags() & ~Qt::ItemIsSelectable);

    for (auto const& exec : action.second)
    {
      QTreeWidgetItem *execItem = new QTreeWidgetItem(topItem);
      execItem->setText(1, exec->toString());
      execItem->setFlags(execItem->flags() & ~Qt::ItemIsSelectable);
    }
  }

  actionTree->resizeColumnToContents(0);
}

void SummaryPage::cleanupPage()
{
  QTreeWidgetItem *item;
  while ((item = actionTree->takeTopLevelItem(0)))
    delete item;
}

ConstExecutionPtrVec SummaryPage::executions() const
{
  ConstExecutionPtrVec result;

  auto const& selectedActions = field("chosenActions").value<ConstActionPtrVec>();
  for (auto const& action : selectedActions)
  {
    auto const& execsForAction = utils::asserted::fromMap(actionExecs, action);
    result.insert(result.end(), execsForAction.begin(), execsForAction.end());
  }

  return result;
}

bool SummaryPage::validatePage()
{
  auto const execs = executions();

  LOG(INFO) << "Finishing process wizard. Returning " << execs.size()
            << " executions for " << actionExecs.size() << " actions";

  emit executionsChanged(execs);
  return true;
}

}} // namespace phobos::processwiz
