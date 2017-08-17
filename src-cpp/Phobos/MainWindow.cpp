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
#include "ProcessWizard/ProcessWizard.h"

namespace phobos {

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    seriesSet(),
    imageCache(seriesSet),
    viewStack(new ViewStack(seriesSet, imageCache))
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
        seriesSet.addSeries(importWizard.selectedSeries());
}

void MainWindow::createMenus()
{
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("&Open"), this, &MainWindow::loadPhotos, QKeySequence("Ctrl+O"))->setStatusTip(tr("Load new photos"));
    // TODO: save option, with possibility to save scaled pixmaps as well, with metrics etc
    // TODO: Load saved config from file, initialize all series, pixmaps, metrics, selections etc, remember to fix UUIDs for series as those will change (or maybe can construct QUuid back from text?
    fileMenu->addSeparator();
    fileMenu->addAction(tr("&Exit"), this, &MainWindow::close, QKeySequence("Ctrl+Q"))->setStatusTip(tr("Exit the application"));

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
    // TODO: Check this. Probably jumping left from first series does not jump to the last one.
    viewMenu->addAction(tr("&Next series"),
                        [this](){ viewStack->handleSwitchView(ViewDescription::make(ViewType::CURRENT, boost::none, +1)); },
                        QKeySequence("Shift+Right"))->setStatusTip(tr("Jump to next series"));
    viewMenu->addAction(tr("&Previous series"),
                        [this](){ viewStack->handleSwitchView(ViewDescription::make(ViewType::CURRENT, boost::none, -1)); },
                        QKeySequence("Shift+Left"))->setStatusTip(tr("Jump to previous series"));

    // TODO: Action: Report -> show dialog with number of series / num selected photos, num unchecked series etc
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

    QMenu* execMenu = menuBar()->addMenu(tr("&Process"));
    execMenu->addAction(tr("&Delete"), this, [this]{processAction(processwiz::OperationType::Delete);})->setStatusTip(tr("Delete selected files from hard drive"));
    execMenu->addAction(tr("&Move"),   this, [this]{processAction(processwiz::OperationType::Move);  })->setStatusTip(tr("Move selected files from hard drive"));
    execMenu->addAction(tr("&Copy"),   this, [this]{processAction(processwiz::OperationType::Copy);  })->setStatusTip(tr("Copy selected files from hard drive"));
    execMenu->addAction(tr("&Rename"), this, [this]{processAction(processwiz::OperationType::Rename);})->setStatusTip(tr("Rename selected files from hard drive"));
}

void MainWindow::processAction(processwiz::OperationType const operation)
{
  processwiz::ProcessWizard processWizard(this, seriesSet, operation);
  if (processWizard.exec())
  {} // TODO: Finish me

/*  auto const selections = viewStack->getSelectionStatus();
  std::vector<QString> toDelete;
  for (auto const& seriesStat : selections.status)
      toDelete.insert(toDelete.end(), seriesStat.discarded.begin(), seriesStat.discarded.end());

  for (QString const& fileName : toDelete)
  {
      LOG(DEBUG) << ":: deleting " << fileName;

      if (remove(fileName.toStdString().c_str()) != 0)
          LOG(ERROR) << "Cannot remove file " << fileName;
  }*/

  // TODO: use trash (or configurable option in dialog) http://stackoverflow.com/questions/17964439/move-files-to-trash-recycle-bin-in-qt
  // TODO: after removing photos something must be done with whole application because loading these from hard drive will be impossible
}

// TODO: Status bar should display percent and fraction of photos(series) viewed, especially in series view

} // namespace phobos
