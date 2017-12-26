#include <QWidget>
#include <QFileDialog>
#include <QDir>
#include <QStandardPaths>
#include <QImageReader>
#include <memory>
#include "ImageOpenDialog.h"

namespace phobos { namespace importwiz {


// TODO: Enable selecting directory and process all photos from it
// TODO: Without directory support, selecting image and directory together cause
// that when accepting selection nothing happens (cannot exit dialog)
// TODO: What happens when invalid files are selected?
namespace {
std::unique_ptr<QFileDialog> createLoadDialog(QWidget *parent)
{
    static bool firstLoadDialog = true;

    std::unique_ptr<QFileDialog> dialog(new QFileDialog(parent, parent->tr("Load photos")));
    if (firstLoadDialog)
    {
        firstLoadDialog = false;
        QStringList const locations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        if (locations.empty())
            dialog->setDirectory(QDir::currentPath());
        else
            dialog->setDirectory(locations.last());
    }

    QStringList supportedTypes;
    for (auto const& mtype : QImageReader::supportedMimeTypes())
        supportedTypes.append(mtype);
    supportedTypes.sort();
    supportedTypes.insert(0, "application/octet-stream");
    dialog->setMimeTypeFilters(supportedTypes);

    dialog->setFileMode(QFileDialog::ExistingFiles);
    return dialog;
}
} // unnamed namespace

QStringList selectImagesInDialog(QWidget *parent)
{
    auto dialog = createLoadDialog(parent);
    if (dialog->exec())
        return dialog->selectedFiles();

    return QStringList();
}

}} // namespace phobos::importwiz
