#include "ProcessWizard/ActionTab.h"
#include "Widgets/IconLabel.h"
#include "Widgets/FilenameEntry.h"
#include "Utils/Filesystem/Trash.h"
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QStyle>
#include <cassert>
#include <easylogging++.h>

namespace phobos { namespace processwiz {

namespace {
class DeleteActionTab : public ActionTab
{
public:
  DeleteActionTab(pcontainer::ItemState const matchState) :
    ActionTab(matchState)
  {
    QVBoxLayout *vlayout = new QVBoxLayout();

    permanentRadio = new QRadioButton(tr("Delete photos permanently"));
    trashRadio = new QRadioButton(tr("Move photos to trash"));
    vlayout->addWidget(permanentRadio);
    vlayout->addWidget(trashRadio);
    vlayout->addStretch(5);

    QHBoxLayout *hlayout = new QHBoxLayout();

    if (utils::fs::trashAvailable())
    {
      trashRadio->setChecked(true);
    }
    else
    {
      permanentRadio->setChecked(true);
      trashRadio->setEnabled(false);
      hlayout->addWidget(new widgets::TextIconLabel(style()->standardIcon(QStyle::SP_MessageBoxInformation),
                                                    tr("Trash is unavailable on this system.")));
    }

    QPushButton *confirmButton = new QPushButton(tr("Create action"));
    QObject::connect(confirmButton, &QPushButton::clicked, this, &DeleteActionTab::createAction);

    hlayout->addStretch();
    hlayout->addWidget(confirmButton);
    vlayout->addLayout(hlayout);

    setLayout(vlayout);
  }

private slots:
  void createAction() const
  {
    auto const method = trashRadio->isChecked()
        ? DeleteAction::Method::Trash
        : DeleteAction::Method::Permanent;

    emit newAction(std::make_shared<DeleteAction>(matchState, method));
  }

private:
  QRadioButton *permanentRadio, *trashRadio;
};

class MoveActionTab : public ActionTab
{
public:
  MoveActionTab(pcontainer::ItemState const matchState) :
    ActionTab(matchState)
  {
  }
};

class CopyActionTab : public ActionTab
{
public:
  CopyActionTab(pcontainer::ItemState const matchState) :
    ActionTab(matchState)
  {
  }
};

class RenameActionTab : public ActionTab
{
public:
  RenameActionTab(pcontainer::ItemState const matchState) :
    ActionTab(matchState)
  {
    QVBoxLayout *vlayout = new QVBoxLayout();

    renameWithSyntax = new widgets::FilenameEntry();
    vlayout->addWidget(renameWithSyntax);

    QPushButton *confirmButton = new QPushButton(tr("Create action"));
    QObject::connect(confirmButton, &QPushButton::clicked, this, &RenameActionTab::createAction);

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->addStretch();
    hlayout->addWidget(confirmButton);
    vlayout->addLayout(hlayout);

    setLayout(vlayout);
  }

private slots:
  void createAction() const
  {
    if (renameWithSyntax->fileNameEdit->hasAcceptableInput())
      emit newAction(std::make_shared<RenameAction>(matchState, renameWithSyntax->unequivocalSyntax()));
    else
      LOG(INFO) << "Attempted to create Rename action from unacceptable input \"" << renameWithSyntax->fileNameEdit->text() << '"';
  }

private:
  widgets::FilenameEntry *renameWithSyntax;
};
} // unnamed namespace

std::unique_ptr<ActionTab> ActionTab::create(OperationType const operation,
                                             pcontainer::ItemState const matchState)
{
  switch(operation)
  {
  case OperationType::Delete:
    return std::make_unique<DeleteActionTab>(matchState);
  case OperationType::Move:
    return std::make_unique<MoveActionTab>(matchState);
  case OperationType::Rename:
    return std::make_unique<RenameActionTab>(matchState);
  case OperationType::Copy:
    return std::make_unique<CopyActionTab>(matchState);
  }

  assert(false);
  return nullptr;
}

ActionTab::ActionTab(pcontainer::ItemState const matchState) :
  matchState(matchState)
{
}

}} // namespace phobos::processwiz
