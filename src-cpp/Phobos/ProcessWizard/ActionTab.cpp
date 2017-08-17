#include "ProcessWizard/ActionTab.h"
#include "Widgets/IconLabel.h"
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QStyle>
#include <cassert>

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

    bool trash = false; // no trash available
    if (trash)
    {
      trashRadio->setChecked(true);
      trashRadio->setEnabled(false);
      hlayout->addWidget(new widgets::TextIconLabel(style()->standardIcon(QStyle::SP_MessageBoxInformation),
                                                    tr("Trash is unavailable on this system.")));
    }
    else
    {
      permanentRadio->setChecked(true);
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

struct RenameWithSyntaxWidget : public QWidget
{
  QLineEdit *fileNameEdit;
  QPushButton *syntaxButton;

  RenameWithSyntaxWidget()
  {
    fileNameEdit = new QLineEdit;
    syntaxButton = new QPushButton(tr("Syntax"));

    // TODO: connect button to some action

    QHBoxLayout *hl1 = new QHBoxLayout();
    hl1->addWidget(new QLabel(tr("New filename:")));
    hl1->addWidget(fileNameEdit);

    QHBoxLayout *hl2 = new QHBoxLayout();
    hl2->addStretch();
    hl2->addWidget(syntaxButton);

    QVBoxLayout *vl = new QVBoxLayout();
    vl->addLayout(hl1);
    vl->addLayout(hl2);

    setLayout(vl);
  }
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

    renameWithSyntax = new RenameWithSyntaxWidget();
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
    emit newAction(std::make_shared<RenameAction>(matchState));
  }

private:
  RenameWithSyntaxWidget *renameWithSyntax;
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
