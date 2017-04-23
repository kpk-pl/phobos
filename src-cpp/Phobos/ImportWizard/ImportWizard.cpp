#include "ImportWizard/ImportWizard.h"
#include "ImportWizard/ImageOpenDialog.h"
#include "ImportWizard/DivisionMethodPage.h"
#include "ImportWizard/SeriesDisplayPage.h"

namespace phobos { namespace importwiz {

ImportWizard::ImportWizard(QWidget *parent) :
    QWizard(parent)
{
    divisionPage = new DivisionMethodPage();
    displayPage = new SeriesDisplayPage();

    addPage(divisionPage);
    addPage(displayPage);

    setWindowTitle(tr("Import wizard"));

    setWindowFlags(windowFlags() | Qt::CustomizeWindowHint);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

void ImportWizard::accept()
{
    // TODO:
    QDialog::accept();
}

PhotoSeriesVec ImportWizard::loadedSeries() const
{
    return divisionPage->series();
}

}} // namespace phobos::importwiz
