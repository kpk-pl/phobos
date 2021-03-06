#include "ImportWizard/ImportWizard.h"
#include "ImportWizard/ImageOpenDialog.h"
#include "ImportWizard/DivisionMethodPage.h"
#include "ImportWizard/SeriesDisplayPage.h"
#include <easylogging++.h>

namespace phobos { namespace importwiz {

ImportWizard::ImportWizard(QWidget *parent) :
    QWizard(parent)
{
  LOG(DEBUG) << "Initializing import wizard";

  divisionPage = new DivisionMethodPage();
  displayPage = new SeriesDisplayPage();

  addPage(divisionPage);
  addPage(displayPage);

  setWindowTitle(tr("Import wizard"));

  setWindowFlags(windowFlags() | Qt::CustomizeWindowHint);
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

  setOptions(QWizard::NoDefaultButton);
  setOption(QWizard::NoDefaultButton, true);
}

PhotoSeriesVec ImportWizard::selectedSeries() const
{
  return field("chosenSeries").value<PhotoSeriesVec>();
}

}} // namespace phobos::importwiz
