#ifndef PROCESSWIZARD_PROCESSWIZARD_H
#define PROCESSWIZARD_PROCESSWIZARD_H

#include "ProcessWizard/Operation.h"
#include "PhotoContainers/Fwd.h"
#include <QWizard>

namespace phobos { namespace processwiz {

class ActionsCreatorPage;

class ProcessWizard : public QWizard
{
  Q_OBJECT
public:
  ProcessWizard(QWidget *parent, pcontainer::Set const& seriesSet, OperationType const defaultOperation);

private:
  pcontainer::Set const& seriesSet;
  ActionsCreatorPage *actionsPage;
};

}} // namespace phobos::processwiz

#endif // PROCESSWIZARD_PROCESSWIZARD_H
