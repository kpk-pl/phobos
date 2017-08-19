#include "ProcessWizard/SummaryPage.h"
#include "ProcessWizard/Action.h"
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

    // TODO: Add action label next to item. This should be a combination of labels for
    // Use item->setIcon(col, Icon);
    // matched type and operation performed on it
    // display icon when there are warnings present or some sort of OK icon if not
    QTreeWidgetItem *topItem = new QTreeWidgetItem(actionTree);
    topItem->setText(0, actionStr);
    topItem->setFlags(topItem->flags() & ~Qt::ItemIsSelectable);

    // TODO: validate each photo from seriesSet agains this action and display warnings
    QTreeWidgetItem *warn = new QTreeWidgetItem(topItem);
    warn->setText(0, tr("Safe to execute"));
    warn->setFlags(warn->flags() & ~Qt::ItemIsSelectable);
  }
}

void SummaryPage::cleanupPage()
{
  QTreeWidgetItem *item;
  while ((item = actionTree->takeTopLevelItem(0)))
      delete item;
}

}} // namespace phobos::processwiz
