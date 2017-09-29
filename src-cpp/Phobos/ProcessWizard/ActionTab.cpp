#include "ProcessWizard/ActionTab.h"
#include "ProcessWizard/ActionsCreatorResources.h"
#include "Widgets/IconLabel.h"
#include "Widgets/FilenameEntry.h"
#include "Utils/Filesystem/Trash.h"
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QLineEdit>
#include <QCheckBox>
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
      hlayout->addWidget(new widgets::TextIconLabel(widgets::IconLabel::Icon::Information,
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

class CopyMoveActionTab : public ActionTab
{
public:
  CopyMoveActionTab(pcontainer::ItemState const matchState,
                    OperationType const operation,
                    ActionsCreatorResources &resources) :
    ActionTab(matchState), operation(operation)
  {
    renameWithSyntax = new widgets::FilenameEntry("NnF", 'N');
    QObject::connect(renameWithSyntax, &widgets::FilenameEntry::helpRequested, &resources, &ActionsCreatorResources::showRenameSyntaxHelp);

    QPushButton *confirmButton = new QPushButton(tr("Create action"));
    QObject::connect(confirmButton, &QPushButton::clicked, this, &CopyMoveActionTab::createAction);
    renameWithSyntax->setSideWidget(confirmButton);
    renameWithSyntax->setEnabled(false);

    QPushButton *dirButton = new QPushButton(tr("Select"));
    //dirButton->setIcon(style()->standardIcon(QStyle::SP_DirOpenIcon));
    QObject::connect(dirButton, &QPushButton::clicked, this, &CopyMoveActionTab::selectDirectory);

    selectedDirLabel = new QLabel(tr("Select directory..."));
    QHBoxLayout *dirLayout = new QHBoxLayout();
    dirLayout->addWidget(selectedDirLabel);
    dirLayout->addStretch();
    dirLayout->addWidget(dirButton);

    renameToggle = new QCheckBox(tr("Rename filename at destination"));
    QObject::connect(renameToggle, &QCheckBox::toggled, renameWithSyntax, &widgets::FilenameEntry::setEnabled);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addLayout(dirLayout);
    layout->addWidget(renameToggle);
    layout->addWidget(renameWithSyntax);
    setLayout(layout);
  }

private slots:
  void createAction() const
  {
    QFileInfo const fInfo(selectedDirLabel->text());
    if (!fInfo.isDir())
    {
      LOG(INFO) << "Attempted to create Copy/Move action with invalid destination \""
                << selectedDirLabel->text() << "\"";
      return;
    }

    QString fPath = fInfo.canonicalPath();
    if (fPath.isEmpty())
      fPath = fInfo.absolutePath();

    QDir const destination(fPath);
    QString optRename;

    if (renameToggle->isChecked())
    {
      if (!renameWithSyntax->fileNameEdit->hasAcceptableInput())
      {
        LOG(INFO) << "Attempted to create Copy/Move action with unacceptable rename pattern \""
                  << renameWithSyntax->fileNameEdit->text() << '"';
        return;
      }
      optRename = renameWithSyntax->unequivocalSyntax();
    }

    switch(operation)
    {
    case OperationType::Copy:
      emit newAction(std::make_shared<CopyAction>(matchState, destination, optRename));
      break;
    case OperationType::Move:
      emit newAction(std::make_shared<MoveAction>(matchState, destination, optRename));
      break;
    default:
      assert(false);
      break;
    }
  }

  void selectDirectory()
  {
    LOG(INFO) << "Displaying dialog to choose copy/move directory";
    QString const dir = QFileDialog::getExistingDirectory(this, tr("Select destination directory"));
    LOG(INFO) << "Selected directory: " << dir;

    // TODO: If text is longer than 50 characters, shorten it like C:/Selected/...bug/etc/dir
    // Try to leave at least first parent dir (Selected) if possible
    // And as meny dirs from tail as possible
    // Maybe even part of last dir that does not fit
    selectedDirLabel->setText(dir);
  }

private:
  OperationType const operation;
  widgets::FilenameEntry *renameWithSyntax;
  QLabel *selectedDirLabel;
  QCheckBox *renameToggle;
};

class RenameActionTab : public ActionTab
{
public:
  RenameActionTab(pcontainer::ItemState const matchState,
                  ActionsCreatorResources &resources) :
    ActionTab(matchState)
  {
    renameWithSyntax = new widgets::FilenameEntry("NnF", 'N');
    QObject::connect(renameWithSyntax, &widgets::FilenameEntry::helpRequested, &resources, &ActionsCreatorResources::showRenameSyntaxHelp);

    QPushButton *confirmButton = new QPushButton(tr("Create action"));
    QObject::connect(confirmButton, &QPushButton::clicked, this, &RenameActionTab::createAction);
    renameWithSyntax->setSideWidget(confirmButton);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(renameWithSyntax);
    setLayout(layout);
  }

private slots:
  void createAction() const
  {
    if (renameWithSyntax->fileNameEdit->hasAcceptableInput())
      emit newAction(std::make_shared<RenameAction>(matchState, renameWithSyntax->unequivocalSyntax()));
    else
      LOG(INFO) << "Attempted to create Rename action from unacceptable input \""
                << renameWithSyntax->fileNameEdit->text() << '"';
  }

private:
  widgets::FilenameEntry *renameWithSyntax;
};
} // unnamed namespace

std::unique_ptr<ActionTab> ActionTab::create(OperationType const operation,
                                             pcontainer::ItemState const matchState,
                                             ActionsCreatorResources &resources)
{
  switch(operation)
  {
  case OperationType::Delete:
    return std::make_unique<DeleteActionTab>(matchState);
  case OperationType::Move:
  case OperationType::Copy:
    return std::make_unique<CopyMoveActionTab>(matchState, operation, resources);
  case OperationType::Rename:
    return std::make_unique<RenameActionTab>(matchState, resources);
  }

  assert(false);
  return nullptr;
}

ActionTab::ActionTab(pcontainer::ItemState const matchState) :
  matchState(matchState)
{
}

}} // namespace phobos::processwiz
