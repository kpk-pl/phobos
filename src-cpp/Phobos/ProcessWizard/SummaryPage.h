#ifndef PROCESSWIZARD_SUMMARYPAGE_H
#define PROCESSWIZARD_SUMMARYPAGE_H

#include "ProcessWizard/ExecutionFwd.h"
#include "ProcessWizard/ActionFwd.h"
#include "ProcessWizard/SeriesCounts.h"
#include "PhotoContainers/Fwd.h"
#include <QWizardPage>
#include <map>

class QTreeWidget;

namespace phobos { namespace processwiz {

class SummaryPage : public QWizardPage
{
  Q_OBJECT
  Q_PROPERTY(phobos::processwiz::ConstExecutionPtrVec executions READ executions NOTIFY executionsChanged STORED false)

public:
  SummaryPage(SeriesCounts const& seriesCounts, pcontainer::Set const& photoSet);

  ConstExecutionPtrVec executions() const;

signals:
  void executionsChanged(ConstExecutionPtrVec);

protected:
  void initializePage() override;
  void cleanupPage() override;
  bool validatePage() override;

private:
  SeriesCounts const seriesCounts;
  pcontainer::Set const& photoSet;

  QTreeWidget *actionTree;

  void updateExecutioners(ConstActionPtrVec const& currentActions);
  std::map<ConstActionPtr, ConstExecutionPtrVec> actionExecs;
};

}} // namespace phobos::processwiz

#endif // PROCESSWIZARD_SUMMARYPAGE_H
