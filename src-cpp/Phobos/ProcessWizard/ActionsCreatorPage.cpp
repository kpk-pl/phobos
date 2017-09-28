#include "ProcessWizard/ActionsCreatorPage.h"
#include "ProcessWizard/TypeActionTab.h"
#include "PhotoContainers/Set.h"
#include "Utils/Asserted.h"
#include "Utils/ItemStateIcon.h"
#include "Utils/LexicalCast.h"
#include "Utils/Algorithm.h"
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QPixmap>
#include <QTextStream>
#include <QTabWidget>
#include <easylogging++.h>

namespace phobos { namespace processwiz {

namespace {
  using ItemState = pcontainer::ItemState;
  static ItemState const allStates[] = { ItemState::SELECTED, ItemState::DISCARDED, ItemState::UNKNOWN };
} // unnamed namespace

ActionsCreatorPage::ActionsCreatorPage(OperationType const& defaultOperation) :
  resources(this)
{
  LOG(DEBUG) << "Creating page";

  setTitle(tr("Processing scheduler"));

  resetButton = new QPushButton(tr("Reset actions"));

  QVBoxLayout *mainLayout = new QVBoxLayout();
  addTypeTabs(mainLayout, defaultOperation);
  addStatusRow(mainLayout);
  setLayout(mainLayout);

  TypeActionTab *currentTab;
  if (defaultOperation == OperationType::Delete)
    currentTab = utils::asserted::fromMap(actionTabs, ItemState::DISCARDED);
  else
    currentTab = utils::asserted::fromMap(actionTabs, ItemState::SELECTED);

  actionTabsWidget->setCurrentWidget(currentTab);

  selectedActionsChanged();

  registerField("chosenActions", this, "chosenActions", SIGNAL(actionsChanged(ConstActionPtrVec)));
}

ConstActionPtrVec ActionsCreatorPage::chosenActions() const
{
  ConstActionPtrVec result;

  for (auto const& at : actionTabs)
  {
    auto active = at.second->activeActions();
    utils::moveFromRange(result, active.begin(), active.end());
  }

  return result;
}

void ActionsCreatorPage::selectedActionsChanged()
{
  auto const actions = chosenActions();
  statusLabel->setText(tr("Scheduled %1 actions to be processed.").arg(actions.size()));
  emit actionsChanged(actions);
}

void ActionsCreatorPage::initializePage()
{
  LOG(INFO) << "Initializing page to create actions";

  if (wizard()->button(QWizard::NextButton))
      wizard()->button(QWizard::NextButton)->setFocus();
}

void ActionsCreatorPage::cleanupPage()
{
  // override default cleanupPage impl because 'chosenActions' field is read-only
  // and cannot be cleared -> suppress warning
}

namespace {
QSize defaultTextSize()
{
  auto const size = QLabel().sizeHint().height();
  return QSize(size, size);
}
} // unnamed namespace

void ActionsCreatorPage::addTypeTabs(QVBoxLayout *layout, OperationType const& defaultOperation)
{
  actionTabsWidget = new QTabWidget();

  for (auto const state : allStates)
  {
    TypeActionTab *typeActionTab = new TypeActionTab(state, resources);
    typeActionTab->setCurrentTab(defaultOperation);
    actionTabs.emplace(state, typeActionTab);
    QObject::connect(typeActionTab, &TypeActionTab::actionsChanged, this, &ActionsCreatorPage::selectedActionsChanged);
    QObject::connect(resetButton, &QPushButton::clicked, typeActionTab, &TypeActionTab::clearActions);

    QString label = QString::fromStdString(utils::lexicalCast(state));
    label[0] = QString(label[0]).toUpper()[0];
    actionTabsWidget->addTab(typeActionTab, utils::itemStateIcon(state, defaultTextSize()), label);
  }

  layout->addWidget(actionTabsWidget);
}

void ActionsCreatorPage::addStatusRow(QVBoxLayout *layout)
{
  statusLabel = new QLabel("");

  QHBoxLayout *hBox = new QHBoxLayout();
  hBox->addWidget(statusLabel);
  hBox->addStretch();
  hBox->addWidget(resetButton);

  layout->addLayout(hBox);
}

}} // namespace phobos::processwiz
