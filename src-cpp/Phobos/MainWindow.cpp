#include <QMenuBar>
#include <QAction>
#include <QKeySequence>
#include <QStandardPaths>
#include <QDir>
#include <QImageReader>
#include <QFileDialog>
#include "MainWindow.h"
#include "Config.h"
#include "ConfigExtension.h"
#include "ViewDescription.h"

namespace phobos {

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    viewStack(new ViewStack()),
    firstLoadDialog(true)

{
    setCentralWidget(viewStack);
    createMenus();

    setWindowTitle(config::qualified<std::string>("mainWindow.title", "Phobos").c_str());
    setMinimumSize(config::qSize("mainWindow.minimumSize", QSize(480, 360)));
    resize(config::qSize("mainWindow.defaultSize", QSize(1024, 768)));
}

void MainWindow::loadPhotos()
{
    QFileDialog* dialog = createLoadDialog();
    if (dialog->exec())
        viewStack->addPhotos(dialog->selectedFiles());
    delete dialog;
}

QFileDialog* MainWindow::createLoadDialog()
{
    QFileDialog* dialog = new QFileDialog(this, tr("Load photos"));
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

void MainWindow::createMenus()
{
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("&Load photos"), this, &MainWindow::loadPhotos, QKeySequence("Ctrl+L"))->setStatusTip(tr("Load new photos"));
    fileMenu->addSeparator();
    fileMenu->addAction(tr("&Exit"), this, &MainWindow::close, QKeySequence("Ctrl+Q"))->setStatusTip(tr("Exit the application"));

    QMenu* viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(tr("&All series"), this,
                        [this](){ viewStack->handleSwitchView(ViewDescription::make(ViewType::ALL_SERIES)); },
                        QKeySequence("Ctrl+A"))->setStatusTip(tr("Show all series in one view"));
    viewMenu->addAction(tr("&One series"), this,
                        [this](){ viewStack->handleSwitchView(ViewDescription::make(ViewType::ROW_SINGLE_SERIES)); },
                        QKeySequence("Ctrl+O"))->setStatusTip(tr("Show one series on a single page"));
    viewMenu->addAction(tr("&Separate photos"), this,
                        [this](){ viewStack->handleSwitchView(ViewDescription::make(ViewType::NUM_SINGLE_SERIES)); },
                        QKeySequence("Ctrl+O"))->setStatusTip(tr("Show separate photos from one series on a single page"));
    viewMenu->addSeparator();
    viewMenu->addAction(tr("&Next series"), this,
                        [this](){ viewStack->handleSwitchView(ViewDescription::make(ViewType::CURRENT, boost::none, +1)); },
                        QKeySequence("Ctrl+N"))->setStatusTip(tr("Jump to next series"));
    viewMenu->addAction(tr("&Previous series"), this,
                        [this](){ viewStack->handleSwitchView(ViewDescription::make(ViewType::CURRENT, boost::none, -1)); },
                        QKeySequence("Ctrl+P"))->setStatusTip(tr("Jump to previous series"));
}
} // namespace phobos
