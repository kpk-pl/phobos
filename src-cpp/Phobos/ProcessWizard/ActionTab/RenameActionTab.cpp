#include "ProcessWizard/ActionTab/RenameActionTab.h"
#include "ProcessWizard/ActionsCreatorResources.h"
#include "ProcessWizard/Action.h"
#include "Widgets/FilenameEntry.h"
#include <easylogging++.h>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>
#include <memory>

namespace phobos { namespace processwiz {

RenameActionTab::RenameActionTab(pcontainer::ItemState const matchState,
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

void RenameActionTab::createAction() const
{
  if (renameWithSyntax->fileNameEdit->hasAcceptableInput())
    emit newAction(std::make_shared<RenameAction>(matchState, renameWithSyntax->unequivocalSyntax()));
  else
    LOG(WARNING) << "Attempted to create Rename action from unacceptable input \""
              << renameWithSyntax->fileNameEdit->text() << '"';
}

}} // namespace phobos::processwiz
