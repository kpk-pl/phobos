#include "ProcessWizard/ActionsCreatorPage.h"
#include "ProcessWizard/Operation.h"
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

ActionsCreatorPage::ActionsCreatorPage(pcontainer::Set const& seriesSet, SeriesCounts const& counts) :
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
}

void ActionsCreatorPage::updateStatusLabel()
{
  std::size_t actions = 0;
  for (TypeActionTab *tab : actionTabs)
    actions += tab->activeActions();

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
  QTabWidget *tabs = new QTabWidget();

  for (auto const state : allStates)
  {
    TypeActionTab *typeActionTab = new TypeActionTab(state);
    actionTabs.push_back(typeActionTab);
    QObject::connect(typeActionTab, &TypeActionTab::actionsChanged, this, &ActionsCreatorPage::updateStatusLabel);
    QObject::connect(resetButton, &QPushButton::clicked, typeActionTab, &TypeActionTab::clearActions);

    QString label = QString::fromStdString(utils::lexicalCast(state));
    label[0] = QString(label[0]).toUpper()[0];
    tabs->addTab(typeActionTab, colorIcon(state), label);
  }

  layout->addWidget(tabs);
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
