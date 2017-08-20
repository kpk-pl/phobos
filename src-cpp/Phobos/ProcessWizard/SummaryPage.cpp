#include "ProcessWizard/SummaryPage.h"
#include "ProcessWizard/Action.h"
#include "ProcessWizard/OperationIcon.h"
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

SummaryPage::SummaryPage()
{
  actionTree = new QTreeWidget();
  actionTree->header()->close();
  actionTree->setColumnCount(2);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(new QLabel(tr("Planned actions:")));
  layout->addWidget(actionTree);
  setLayout(layout);
}

void SummaryPage::initializePage()
{
  auto const selectedActions = field("chosenActions").value<ConstActionPtrVec>();
  LOG(INFO) << "Displaying summary for " << selectedActions.size() << " selected actions";

  for (auto const& action : selectedActions)
  {
    auto const actionStr = action->toString();
    LOG(INFO) << "Enabled: " << actionStr;

    QTreeWidgetItem *topItem = new QTreeWidgetItem(actionTree);
    topItem->setIcon(0, operationIcon(action->operation()));
    topItem->setText(1, actionStr);
    topItem->setFlags(topItem->flags() & ~Qt::ItemIsSelectable);

    // TODO: validate each photo from seriesSet agains this action and display warnings
    QTreeWidgetItem *warn = new QTreeWidgetItem(topItem);
    warn->setText(1, tr("Safe to execute"));
    warn->setFlags(warn->flags() & ~Qt::ItemIsSelectable);
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