#include <QMenuBar>
#include <QAction>
#include <QKeySequence>
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
    importWizard.exec();

    if (!importWizard.selectedFiles().empty())
        viewStack->addPhotos(importWizard.selectedFiles());
}

void MainWindow::createMenus()
{
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("&Open"), this, &MainWindow::loadPhotos, QKeySequence("Ctrl+O"))->setStatusTip(tr("Load new photos"));
    // TODO: save option, with possibility to save scaled pixmaps as well, with metrics etc
    // TODO: Load saved config from file, initialize all series, pixmaps, metrics, selections etc, remember to fix UUIDs for series as those will change (or maybe can construct QUuid back from text?
    fileMenu->addSeparator();
    fileMenu->addAction(tr("&Exit"), this, &MainWindow::close, QKeySequence("Ctrl+Q"))->setStatusTip(tr("Exit the application"));

    QMenu* actionMenu = menuBar()->addMenu(tr("&Action"));
    actionMenu->addAction(tr("&Select best"), this,
            [this](){ viewStack->bulkSelect(PhotoBulkAction::SELECT_BEST); }
            )->setStatusTip(tr("Select best photos in each series"));
    actionMenu->addAction(tr("&Select unchecked"), this,
            [this](){ viewStack->bulkSelect(PhotoBulkAction::SELECT_UNCHECKED); }
            )->setStatusTip(tr("Select all unchecked photos"));
    actionMenu->addAction(tr("&Invert selection"), this,
            [this](){ viewStack->bulkSelect(PhotoBulkAction::INVERT); }
            )->setStatusTip(tr("Invert selection"));
    actionMenu->addAction(tr("&Clear selection"), this,
            [this](){ viewStack->bulkSelect(PhotoBulkAction::CLEAR); }
            )->setStatusTip(tr("Clear selection"));
    // TODO: Action: Report -> show dialog with number of series / num selected photos, num unchecked series etc
    // TODO: Action: Remove selected
    // TODO: Action: Move selected
    // TODO: Action: Copy selected
    //
    // TODO: to viewMenubar add selectable options to enable/disable addons on photoitemwidgets

    QMenu* viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(tr("&All series"), this,
                        [this](){ viewStack->handleSwitchView(ViewDescription::make(ViewType::ALL_SERIES)); },
                        QKeySequence("Alt+1"))->setStatusTip(tr("Show all series in one view"));
    viewMenu->addAction(tr("&One series"), this,
                        [this](){ viewStack->handleSwitchView(ViewDescription::make(ViewType::ROW_SINGLE_SERIES)); },
                        QKeySequence("Alt+2"))->setStatusTip(tr("Show one series on a single page"));
    viewMenu->addAction(tr("&Separate photos"), this,
                        [this](){ viewStack->handleSwitchView(ViewDescription::make(ViewType::NUM_SINGLE_SERIES)); },
                        QKeySequence("Alt+3"))->setStatusTip(tr("Show separate photos from one series on a single page"));
    viewMenu->addSeparator();
    viewMenu->addAction(tr("&Next series"), this,
                        [this](){ viewStack->handleSwitchView(ViewDescription::make(ViewType::CURRENT, boost::none, +1)); },
                        QKeySequence("Ctrl+N"))->setStatusTip(tr("Jump to next series"));
    viewMenu->addAction(tr("&Previous series"), this,
                        [this](){ viewStack->handleSwitchView(ViewDescription::make(ViewType::CURRENT, boost::none, -1)); },
                        QKeySequence("Ctrl+P"))->setStatusTip(tr("Jump to previous series"));
}
} // namespace phobos
