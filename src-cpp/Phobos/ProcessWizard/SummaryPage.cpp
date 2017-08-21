#include "ProcessWizard/SummaryPage.h"
#include "ProcessWizard/Action.h"
#include "ProcessWizard/OperationIcon.h"
#include "ProcessWizard/Execution.h"
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
}

void SummaryPage::updateExecutioners(ConstActionPtrVec const& currentActions)
{
  for (auto it = executioners.begin(); it != executioners.end(); )
  {
    if (std::find(currentActions.begin(), currentActions.end(), it->first) != currentActions.end())
      ++it;
    else
      it = executioners.erase(it);
  }

  if (executioners.size() == currentActions.size())
    return;

  for (auto const& action : currentActions)
    if (executioners.find(action) == executioners.end())
      executioners.emplace(action, action->makeExecutions(photoSet, seriesCounts));
}

void SummaryPage::initializePage()
{
  LOG(INFO) << "Initializing summary page";

  auto const selectedActions = field("chosenActions").value<ConstActionPtrVec>();
  LOG(INFO) << "Displaying summary for " << selectedActions.size() << " selected actions";

  updateExecutioners(selectedActions);

  for (auto const& action : executioners)
  {
    auto const actionStr = action.first->toString();
    LOG(INFO) << "Enabled: " << actionStr;

    QTreeWidgetItem *topItem = new QTreeWidgetItem(actionTree);
    topItem->setIcon(0, operationIcon(action.first->operation()));
    topItem->setText(1, actionStr);
    topItem->setFlags(topItem->flags() & ~Qt::ItemIsSelectable);

    for (auto const& exec : *action.second)
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

bool SummaryPage::validatePage()
{
  LOG(INFO) << "Finishing process wizard. Executing " << executioners.size() << " actions";

  std::size_t errors = 0;

  for (auto const& action : executioners)
  {
    auto const actionStr = action.first->toString();
    LOG(INFO) << "Executing action: " << actionStr;

    for (auto const& exec : *action.second)
    {
      if (!exec->execute())
      {
        LOG(WARNING) << "Failed: " << exec->toString();
        ++errors;
      }
    }
  }

  LOG(INFO) << "Performed all actions with " << errors << " errors";

  return true;
}

}} // namespace phobos::processwiz
