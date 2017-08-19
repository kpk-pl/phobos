#include "ProcessWizard/ActionsCreatorPage.h"
#include "ProcessWizard/TypeActionTab.h"
#include "PhotoContainers/Set.h"
#include "Utils/Asserted.h"
#include "Utils/ItemStateColor.h"
#include "Utils/LexicalCast.h"
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

ActionsCreatorPage::ActionsCreatorPage(SeriesCounts const& counts, OperationType const& defaultOperation) :
  seriesCounts(counts)
{
  LOG(INFO) << "Initializing page to create actions";

  setTitle(tr("Processing scheduler"));
  setButtonText(QWizard::WizardButton::FinishButton, tr("Execute"));

  resetButton = new QPushButton(tr("Reset actions"));

  QVBoxLayout *mainLayout = new QVBoxLayout();
  addTypeTabs(mainLayout);
  addStatusRow(mainLayout);
  setLayout(mainLayout);

  TypeActionTab *currentTab;
  if (defaultOperation == OperationType::Delete)
    currentTab = utils::asserted::fromMap(actionTabs, ItemState::DISCARDED);
  else
    currentTab = utils::asserted::fromMap(actionTabs, ItemState::SELECTED);

  actionTabsWidget->setCurrentWidget(currentTab);
  currentTab->setCurrentTab(defaultOperation);
}

void ActionsCreatorPage::updateStatusLabel()
{
  std::size_t actions = 0;
  for (auto const& at : actionTabs)
    actions += at.second->activeActions();

  statusLabel->setText(tr("Scheduled %1 actions to be processed.").arg(actions));
}

void ActionsCreatorPage::initializePage()
{
  if (wizard()->button(QWizard::NextButton))
      wizard()->button(QWizard::NextButton)->setFocus();
  else if (wizard()->button(QWizard::FinishButton))
      wizard()->button(QWizard::FinishButton)->setFocus();
}

namespace {
QIcon colorIcon(pcontainer::ItemState const state)
{
  auto const size = QLabel().sizeHint().height();
  QPixmap colorBox(size, size);
  colorBox.fill(utils::itemStateColor(state));
  return QIcon(colorBox);
}
} // unnamed namespace

void ActionsCreatorPage::addTypeTabs(QVBoxLayout *layout)
{
  actionTabsWidget = new QTabWidget();

  for (auto const state : allStates)
  {
    TypeActionTab *typeActionTab = new TypeActionTab(state);
    actionTabs.emplace(state, typeActionTab);
    QObject::connect(typeActionTab, &TypeActionTab::actionsChanged, this, &ActionsCreatorPage::updateStatusLabel);
    QObject::connect(resetButton, &QPushButton::clicked, typeActionTab, &TypeActionTab::clearActions);

    QString label = QString::fromStdString(utils::lexicalCast(state));
    label[0] = QString(label[0]).toUpper()[0];
    actionTabsWidget->addTab(typeActionTab, colorIcon(state), label);
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

  updateStatusLabel();

  layout->addLayout(hBox);
}

}} // namespace phobos::processwiz
