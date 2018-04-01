#include "ProcessWizard/ActionsCreatorResources.h"
#include <QDialog>
#include <QLabel>
#include <QString>
#include <QGridLayout>
#include <easylogging++.h>

namespace phobos { namespace processwiz {

ActionsCreatorResources::ActionsCreatorResources(QWidget *parent) :
  parent(parent)
{}

void ActionsCreatorResources::showRenameSyntaxHelp()
{
  createRenameSyntaxDialog();

  LOG(TRACE) << "Displaying rename syntax dialog";
  renameSyntaxDialog->show();
  renameSyntaxDialog->raise();
  renameSyntaxDialog->activateWindow();
}

void ActionsCreatorResources::createRenameSyntaxDialog()
{
  if (renameSyntaxDialog)
    return;

  LOG(DEBUG) << "Creating rename syntax dialog";
  renameSyntaxDialog = new QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
  renameSyntaxDialog->setWindowTitle(tr("File rename syntax help"));

  QGridLayout *layout = new QGridLayout();
  renameSyntaxDialog->setLayout(layout);

  QLabel *desc = new QLabel(tr("The following placeholders will be replaced by appropriate values for each photo."));
  desc->setWordWrap(true);
  layout->addWidget(desc, 0, 0, 1, 2);

  layout->addWidget(new QLabel("<b><pre>%F</pre></b>"), 1, 0);
  layout->addWidget(new QLabel(tr(" - original filename")), 1, 1);

  layout->addWidget(new QLabel("<b><pre>%N</pre></b>"), 2, 0);
  layout->addWidget(new QLabel(tr(" - photo number, taking into account all photos")), 2, 1);

  layout->addWidget(new QLabel("<b><pre>%n</pre></b>"), 3, 0);
  layout->addWidget(new QLabel(tr(" - photo number, taking into account only photos of the same category")), 3, 1);
}

}} // namespace phobos::processwiz
