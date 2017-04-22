#include "ImportWizard/ImportWizard.h"
#include "ImportWizard/ImageOpenDialog.h"
#include "ImportWizard/DivisionMethodPage.h"

namespace phobos { namespace importwiz {

ImportWizard::ImportWizard(QWidget *parent) :
    QWizard(parent)
{
    addPage(new DivisionMethodPage(this));
    setWindowTitle(tr("Import wizard"));

    setWindowFlags(windowFlags() | Qt::CustomizeWindowHint);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

int ImportWizard::exec()
{
    loadMoreFiles();
    return QWizard::exec();
}

void ImportWizard::accept()
{
    QDialog::accept();
}

void ImportWizard::loadMoreFiles()
{
    QStringList const newFiles = selectImagesInDialog(this);
    _selectedFiles.append(newFiles);
    // TODO: remove duplicates
}

}} // namespace phobos::importwiz
