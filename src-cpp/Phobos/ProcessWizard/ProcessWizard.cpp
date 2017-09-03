#include "ProcessWizard/ProcessWizard.h"
#include "ProcessWizard/ActionsCreatorPage.h"
#include "ProcessWizard/SummaryPage.h"
#include "ProcessWizard/WarningsPage.h"
#include "ProcessWizard/SeriesCounts.h"
#include "ProcessWizard/Execution.h"
#include <easylogging++.h>

namespace phobos { namespace processwiz {

ProcessWizard::ProcessWizard(QWidget *parent, pcontainer::Set const& seriesSet, OperationType const defaultOperation) :
  QWizard(parent)
{
  LOG(INFO) << "Creating processing wizard with default operation \"" << defaultOperation << '"';

  SeriesCounts const counts = countPhotos(seriesSet);

  addPage(new WarningsPage(counts));
  addPage(new ActionsCreatorPage(defaultOperation));
  addPage(new SummaryPage(counts, seriesSet));

  setWindowTitle(tr("Processing wizard"));

  setWindowFlags(windowFlags() | Qt::CustomizeWindowHint);
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

  setOptions(QWizard::NoDefaultButton);
  setOption(QWizard::NoDefaultButton, true);
}

ConstActionPtrVec ProcessWizard::createdActions() const
{
  return field("choosenActions").value<ConstActionPtrVec>();
}

ConstExecutionPtrVec ProcessWizard::executions() const
{
  return field("executions").value<ConstExecutionPtrVec>();
}

}} // namespace phobos::processwiz
