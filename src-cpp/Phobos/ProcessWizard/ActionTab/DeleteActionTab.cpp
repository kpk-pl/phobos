#include "ProcessWizard/ActionTab/DeleteActionTab.h"
#include "ProcessWizard/Action.h"
#include "Utils/Filesystem/Trash.h"
#include "Widgets/IconLabel.h"
#include <QRadioButton>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <memory>

namespace phobos { namespace processwiz {

DeleteActionTab::DeleteActionTab(pcontainer::ItemState const matchState) :
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

void DeleteActionTab::createAction() const
{
  auto const method = trashRadio->isChecked()
      ? DeleteAction::Method::Trash
      : DeleteAction::Method::Permanent;

  emit newAction(std::make_shared<DeleteAction>(matchState, method));
}

}} // namespace phobos::processwiz
