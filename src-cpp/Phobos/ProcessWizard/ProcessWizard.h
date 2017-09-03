#ifndef PROCESSWIZARD_PROCESSWIZARD_H
#define PROCESSWIZARD_PROCESSWIZARD_H

#include "ProcessWizard/Operation.h"
#include "ProcessWizard/ActionFwd.h"
#include "ProcessWizard/ExecutionFwd.h"
#include "PhotoContainers/Fwd.h"
#include <QWizard>

namespace phobos { namespace processwiz {

class ProcessWizard : public QWizard
{
  Q_OBJECT
public:
  ProcessWizard(QWidget *parent, pcontainer::Set const& seriesSet, OperationType const defaultOperation);

  ConstActionPtrVec createdActions() const;
  ConstExecutionPtrVec executions() const;
};

}} // namespace phobos::processwiz

#endif // PROCESSWIZARD_PROCESSWIZARD_H
