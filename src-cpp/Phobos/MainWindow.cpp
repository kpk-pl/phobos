#include <cstdio>
#include <QMenuBar>
#include <QAction>
#include <QKeySequence>
#include <easylogging++.h>
#include "MainWindow.h"
#include "Config.h"
#include "ConfigExtension.h"
#include "ViewDescription.h"
#include "PhotoBulkAction.h"
#include "ImportWizard/ImportWizard.h"

namespace phobos {

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    viewStack(new ViewStack())
{
    setCentralWidget(viewStack);
    createMenus();

    setWindowTitle(config::qualified<std::string>("mainWindow.title", "Phobos").c_str());
    setMinimumSize(config::qSize("mainWindow.minimumSize", QSize(480, 360)));
    resize(config::qSize("mainWindow.defaultSize", QSize(1024, 768)));
}

void MainWindow::loadPhotos()
{
    importwiz::ImportWizard importWizard(this);
    if (importWizard.exec())
        viewStack->addPhotos(importWizard.selectedSeries());
}

void MainWindow::createMenus()
{
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("&Open"), this, &MainWindow::loadPhotos, QKeySequence("Ctrl+O"))->setStatusTip(tr("Load new photos"));
    // TODO: save option, with possibility to save scaled pixmaps as well, with metrics etc
    // TODO: Load saved config from file, initialize all series, pixmaps, metrics, selections etc, remember to fix UUIDs for series as those will change (or maybe can construct QUuid back from text?
    fileMenu->addSeparator();
    fileMenu->addAction(tr("&Exit"), this, &MainWindow::close, QKeySequence("Ctrl+Q"))->setStatusTip(tr("Exit the application"));

    QMenu* execMenu = menuBar()->addMenu(tr("&Execute"));
    execMenu->addAction(tr("&Remove"), this, &MainWindow::removeSelected)->setStatusTip(tr("Remove selected files from hard drive"));
    execMenu->addAction(tr("&Move"), this, &MainWindow::moveSelected)->setStatusTip(tr("Move selected files to another directory"));
    execMenu->addAction(tr("&Copy"), this, &MainWindow::copySelected)->setStatusTip(tr("Copy selected files to another directory"));

    QMenu* actionMenu = menuBar()->addMenu(tr("&Action"));
    actionMenu->addAction(tr("Select &best"), [this](){ viewStack->bulkSelect(PhotoBulkAction::SELECT_BEST); })
            ->setStatusTip(tr("Select best photos in each series"));
    actionMenu->addAction(tr("Select &unchecked"), [this](){ viewStack->bulkSelect(PhotoBulkAction::SELECT_UNCHECKED); })
            ->setStatusTip(tr("Select all unchecked photos"));
    actionMenu->addAction(tr("&Discard unchecked"), [this](){ viewStack->bulkSelect(PhotoBulkAction::DISCARD_UNCHECKED); })
            ->setStatusTip(tr("Select all unchecked photos"));
    actionMenu->addAction(tr("&Invert selection"), [this](){ viewStack->bulkSelect(PhotoBulkAction::INVERT); })
            ->setStatusTip(tr("Invert selection"));
    actionMenu->addAction(tr("&Clear selection"), [this](){ viewStack->bulkSelect(PhotoBulkAction::CLEAR); })
            ->setStatusTip(tr("Clear selection"));

    // TODO: Action: Report -> show dialog with number of series / num selected photos, num unchecked series etc
    // TODO: to viewMenubar add selectable options to enable/disable addons on photoitemwidgets

    QMenu* viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(tr("&All series"),
                        [this](){ viewStack->handleSwitchView(ViewDescription::make(ViewType::ALL_SERIES)); },
                        QKeySequence("Alt+1"))->setStatusTip(tr("Show all series in one view"));
    viewMenu->addAction(tr("&One series"),
                        [this](){ viewStack->handleSwitchView(ViewDescription::make(ViewType::ROW_SINGLE_SERIES)); },
                        QKeySequence("Alt+2"))->setStatusTip(tr("Show one series on a single page"));
    viewMenu->addAction(tr("&Separate photos"),
                        [this](){ viewStack->handleSwitchView(ViewDescription::make(ViewType::NUM_SINGLE_SERIES)); },
                        QKeySequence("Alt+3"))->setStatusTip(tr("Show separate photos from one series on a single page"));
    viewMenu->addSeparator();
    viewMenu->addAction(tr("&Next series"),
                        [this](){ viewStack->handleSwitchView(ViewDescription::make(ViewType::CURRENT, boost::none, +1)); },
                        QKeySequence("Ctrl+N"))->setStatusTip(tr("Jump to next series"));
    viewMenu->addAction(tr("&Previous series"),
                        [this](){ viewStack->handleSwitchView(ViewDescription::make(ViewType::CURRENT, boost::none, -1)); },
                        QKeySequence("Ctrl+P"))->setStatusTip(tr("Jump to previous series"));
}

void MainWindow::removeSelected() const
{
    auto const selections = viewStack->getSelectionStatus();
    std::vector<std::string> toDelete;
    for (auto const& seriesStat : selections.status)
        toDelete.insert(toDelete.end(), seriesStat.discarded.begin(), seriesStat.discarded.end());

    for (auto const& fileName : toDelete)
    {
        LOG(DEBUG) << ":: deleting " << fileName;

        if (remove(fileName.c_str()) != 0)
            LOG(ERROR) << "Cannot remove file " << fileName;
    }
}

void MainWindow::moveSelected() const
{

}

void MainWindow::copySelected() const
{

}

} // namespace phobos
