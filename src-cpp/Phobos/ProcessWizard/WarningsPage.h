#ifndef PROCESSWIZARD_WARNINGSPAGE_H
#define PROCESSWIZARD_WARNINGSPAGE_H

#include "ProcessWizard/SeriesCounts.h"
#include <QWizardPage>

namespace phobos { namespace processwiz {

class WarningsPage : public QWizardPage
{
public:
  WarningsPage(SeriesCounts const& counts);

protected:
  void initializePage() override;
  bool validatePage() override;
};

}} // namespace phobos::processwiz

#endif // PROCESSWIZARD_WARNINGSPAGE_H
