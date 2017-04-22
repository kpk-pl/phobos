#include "ImportWizard/ImportWizard.h"
#include "ImportWizard/ImageOpenDialog.h"
#include "ImportWizard/DivisionMethodPage.h"

namespace phobos { namespace importwiz {

ImportWizard::ImportWizard(QWidget *parent) :
    QWizard(parent)
{
    divisionPage = new DivisionMethodPage(this);
    addPage(divisionPage);
    setWindowTitle(tr("Import wizard"));

    setWindowFlags(windowFlags() | Qt::CustomizeWindowHint);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

void ImportWizard::accept()
{
    // TODO:
    QDialog::accept();
}

std::vector<PhotoSeries> const& ImportWizard::loadedSeries() const
{
    return divisionPage->series();
}

}} // namespace phobos::importwiz
