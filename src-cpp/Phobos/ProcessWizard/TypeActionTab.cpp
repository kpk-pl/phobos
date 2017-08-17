#include "ProcessWizard/TypeActionTab.h"
#include "ProcessWizard/ActionTab.h"
#include "ProcessWizard/Operation.h"
#include <QVBoxLayout>
#include <QTabWidget>
#include <QListWidget>
#include <easylogging++.h>
#include <cassert>

namespace phobos { namespace processwiz {

TypeActionTab::TypeActionTab(pcontainer::ItemState const matchState)
{
  QVBoxLayout *layout = new QVBoxLayout();

  listWidget = new QListWidget();
  QObject::connect(listWidget, &QListWidget::itemClicked, this, &TypeActionTab::selectivelyDisableActions);
  layout->addWidget(listWidget);

  QTabWidget *tabs = new QTabWidget();
  layout->addWidget(tabs);

  auto const addTab = [tabs, matchState, this](OperationType const operation, QString const& label){
    auto actionTab = ActionTab::create(operation, matchState);
    QObject::connect(actionTab.get(), &ActionTab::newAction, this, &TypeActionTab::acceptNewAction);
    tabs->addTab(actionTab.release(), label);
  };

  addTab(OperationType::Delete, tr("Delete"));
  addTab(OperationType::Move, tr("Move"));
  addTab(OperationType::Copy, tr("Copy"));
  addTab(OperationType::Rename, tr("Rename"));

  setLayout(layout);
}

struct TypeActionTab::ActionWithOrderComp
{
  bool operator()(ActionWithOrder const& lhs, ActionWithOrder const& rhs)
  {
    auto const lhs_p = lhs.first->priority();
    auto const rhs_p = rhs.first->priority();

    return (lhs_p != rhs_p) ? (lhs_p < rhs_p) : (lhs.second < rhs.second);
  }
};

namespace {
  bool isItemEnabled(QListWidgetItem *item)
  {
    return item->flags() & Qt::ItemIsEnabled;
  }
  void enableItem(QListWidgetItem *item)
  {
    item->setFlags(item->flags() | Qt::ItemIsEnabled);
  }
  void disableItem(QListWidgetItem *item)
  {
    item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
  }
} // unnamed namespace

void TypeActionTab::acceptNewAction(ConstActionPtr action)
{
  actions.emplace_back(action, actions.size());

  std::sort(actions.begin(), actions.end(), ActionWithOrderComp{});
  auto const newActionIt = std::find_if(actions.begin(), actions.end(),
                                        [&action](auto const& p){ return p.first == action; });
  assert(newActionIt != actions.end());
  auto const newActionPos = std::distance(actions.begin(), newActionIt);

  QListWidgetItem *newItem = new QListWidgetItem(action->toString());
  newItem->setFlags(newItem->flags() | Qt::ItemIsUserCheckable);
  newItem->setCheckState(Qt::Unchecked);
  listWidget->insertItem(newActionPos, newItem);
  LOG(INFO) << "Created new action \"" << action->toString()
            << "\" for " << action->matching() << " items on position " << newActionPos;

  selectivelyDisableActions();

  if(isItemEnabled(newItem))
  {
    newItem->setCheckState(Qt::Checked);
    selectivelyDisableActions();
  }

  emit actionsChanged();
}

void TypeActionTab::selectivelyDisableActions()
{
  assert(actions.size() == static_cast<unsigned>(listWidget->count()));

  bool consumed = false;
  for (std::size_t i = 0; i < actions.size(); ++i)
  {
    QListWidgetItem *item = listWidget->item(i);
    if (!consumed)
    {
      enableItem(item);
      if (item->checkState() == Qt::Checked)
        consumed |= actions[i].first->greedy();
    }
    else
      disableItem(item);
  }
}

void TypeActionTab::clearActions()
{
  actions.clear();

  while(listWidget->count() > 0)
    delete listWidget->takeItem(0);

  emit actionsChanged();
}

std::size_t TypeActionTab::activeActions() const
{
  return actions.size();
}

}} // namespace phobos::processwiz
