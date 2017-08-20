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
  auto const selectedActions = field("chosenActions").value<ConstActionPtrVec>();
  LOG(INFO) << "Displaying summary for " << selectedActions.size() << " selected actions";

  updateExecutioners(selectedActions);

  for (auto const& action : selectedActions)
  {
    auto const actionStr = action->toString();
    LOG(INFO) << "Enabled: " << actionStr;

    QTreeWidgetItem *topItem = new QTreeWidgetItem(actionTree);
    topItem->setIcon(0, operationIcon(action->operation()));
    topItem->setText(1, actionStr);
    topItem->setFlags(topItem->flags() & ~Qt::ItemIsSelectable);

    for (auto const& exec : *utils::asserted::fromMap(executioners, action))
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

}} // namespace phobos::processwiz
