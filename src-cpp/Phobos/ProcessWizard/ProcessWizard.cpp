#include "ProcessWizard/ProcessWizard.h"
#include "ProcessWizard/ActionsCreatorPage.h"
#include "ProcessWizard/WarningsPage.h"
#include "ProcessWizard/SeriesCounts.h"
#include <easylogging++.h>

namespace phobos { namespace processwiz {

ProcessWizard::ProcessWizard(QWidget *parent, pcontainer::Set const& seriesSet, OperationType const defaultOperation) :
  QWizard(parent)
{
  LOG(INFO) << "Creating processing wizard with default operation \"" << defaultOperation << '"';

  SeriesCounts const counts = countPhotos(seriesSet);

  addPage(new WarningsPage(counts));
  addPage(new ActionsCreatorPage(counts, defaultOperation));

  setWindowTitle(tr("Processing wizard"));

  setWindowFlags(windowFlags() | Qt::CustomizeWindowHint);
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

  setOptions(QWizard::NoDefaultButton);
  setOption(QWizard::NoDefaultButton, true);
}

ConstActionPtrVec ProcessWizard::createdActions() const
{
  return field("createdActions").value<ConstActionPtrVec>();
}

}} // namespace phobos::processwiz
