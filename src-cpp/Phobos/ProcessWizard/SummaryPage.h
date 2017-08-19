#ifndef PROCESSWIZARD_SUMMARYPAGE_H
#define PROCESSWIZARD_SUMMARYPAGE_H

#include <QWizardPage>

class QTreeWidget;

namespace phobos { namespace processwiz {

class SummaryPage : public QWizardPage
{
  Q_OBJECT

public:
  SummaryPage();

protected:
  void initializePage() override;
  void cleanupPage() override;

private:
  QTreeWidget *actionTree;
};

}} // namespace phobos::processwiz

#endif // PROCESSWIZARD_SUMMARYPAGE_H
