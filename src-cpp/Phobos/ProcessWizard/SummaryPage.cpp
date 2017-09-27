#include "ProcessWizard/SummaryPage.h"
#include "ProcessWizard/Action.h"
#include "ProcessWizard/OperationIcon.h"
#include "ProcessWizard/Execution.h"
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
