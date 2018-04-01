#include "ProcessWizard/ActionTab/CopyMoveActionTab.h"
#include "ProcessWizard/ActionsCreatorResources.h"
#include "ProcessWizard/Action.h"
#include "Widgets/FilenameEntry.h"
#include <easylogging++.h>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QFileInfo>
#include <QDir>
#include <QFileDialog>
#include <QLineEdit>
#include <QString>
#include <memory>
#include <cassert>

namespace phobos { namespace processwiz {

CopyMoveActionTab::CopyMoveActionTab(pcontainer::ItemState const matchState,
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
void CopyMoveActionTab::createAction() const
{
  QFileInfo const fInfo(selectedDirLabel->text());
  if (!fInfo.isDir())
  {
    LOG(WARNING) << "Attempted to create Copy/Move action with invalid destination \""
              << selectedDirLabel->text() << "\"";
    return;
  }

  QString fPath = fInfo.canonicalFilePath();
  if (fPath.isEmpty())
    fPath = fInfo.absoluteFilePath();

  QDir const destination(fPath);
  QString optRename;

  if (renameToggle->isChecked())
  {
    if (!renameWithSyntax->fileNameEdit->hasAcceptableInput())
    {
      LOG(WARNING) << "Attempted to create Copy/Move action with unacceptable rename pattern \""
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

void CopyMoveActionTab::selectDirectory()
{
  LOG(TRACE) << "Displaying dialog to choose copy/move directory";
  QString const dir = QFileDialog::getExistingDirectory(this, tr("Select destination directory"));
  LOG(TRACE) << "Selected directory: " << dir;

  // TODO: If text is longer than 50 characters, shorten it like C:/Selected/...bug/etc/dir
  // Try to leave at least first parent dir (Selected) if possible
  // And as meny dirs from tail as possible
  // Maybe even part of last dir that does not fit
  selectedDirLabel->setText(dir);
}

}} // namespace phobos::processwiz
