#include "ProcessWizard/ProcessWizard.h"
#include "ProcessWizard/ActionsCreatorPage.h"
#include "ProcessWizard/WarningsPage.h"
#include "ProcessWizard/SeriesCounts.h"
#include <easylogging++.h>

namespace phobos { namespace processwiz {

ProcessWizard::ProcessWizard(QWidget *parent, pcontainer::Set const& seriesSet, OperationType const defaultOperation) :
  QWizard(parent), seriesSet(seriesSet)
{
  LOG(INFO) << "Creating processing wizard with default operation \"" << defaultOperation << '"';

  SeriesCounts const counts = countPhotos(seriesSet);

  addPage(new WarningsPage(counts));

  actionsPage = new ActionsCreatorPage(seriesSet, counts);
  addPage(actionsPage);

  setWindowTitle(tr("Processing wizard"));

  setWindowFlags(windowFlags() | Qt::CustomizeWindowHint);
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

  setOptions(QWizard::NoDefaultButton);
  setOption(QWizard::NoDefaultButton, true);
}

}} // namespace phobos::processwiz
