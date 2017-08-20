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

public:
  SummaryPage(SeriesCounts const& seriesCounts, pcontainer::Set const& photoSet);

protected:
  void initializePage() override;
  void cleanupPage() override;

private:
  SeriesCounts const seriesCounts;
  pcontainer::Set const& photoSet;

  QTreeWidget *actionTree;

  void updateExecutioners(ConstActionPtrVec const& currentActions);
  std::map<ConstActionPtr, ConstExecutionPtrVecConstPtr> executioners;
};

}} // namespace phobos::processwiz

#endif // PROCESSWIZARD_SUMMARYPAGE_H
