#include "MainWindow.h"
#include "ViewStack.h"
#include "Config.h"
#include "ConfigExtension.h"
#include "ViewDescription.h"
#include "PhotoBulkAction.h"
#include "MainToolbar.h"
#include "ImportWizard/ImportWizard.h"
#include "ProcessWizard/ProcessWizard.h"
#include "ProcessWizard/Execution/Execution.h"
#include "ProcessWizard/Execution/Execute.h"
#include "Widgets/StatusBarSlider.h"
#include "Widgets/StatusBarLeftRightNavigation.h"
#include "Widgets/Toolbar/Signal.h"
#include "ImageProcessing/Enhance/OperationType.h"
#include "Utils/Focused.h"
#include "Utils/FilenameChooser.h"
#include <easylogging++.h>
#include <QApplication>
#include <QAction>
#include <QCloseEvent>
#include <QMessageBox>
#include <QKeySequence>
#include <QVBoxLayout>
#include <QToolButton>
#include <QStatusBar>
#include <cstdio>

namespace phobos {

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  seriesSet(),
  imageCache(seriesSet),
  sharedWidgets(),
  mainToolbar(new MainToolbar),
  viewStack(new ViewStack(seriesSet, imageCache, sharedWidgets, mainToolbar)),
  enhanceFilenameChooser(new utils::FilenameChooser(this))
{
  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->addWidget(mainToolbar);
  mainLayout->addWidget(viewStack);

  QWidget *mainWidget = new QWidget;
  mainWidget->setLayout(mainLayout);
  setCentralWidget(mainWidget);

  connectToolbar();
  configureStatusBar();

  setWindowTitle(config::qualified<std::string>("mainWindow.title", "Phobos").c_str());
  setMinimumSize(config::qSize("mainWindow.minimumSize", QSize(480, 360)));
  resize(config::qSize("mainWindow.defaultSize", QSize(1024, 768)));
}

void MainWindow::closeEvent(QCloseEvent *ev)
{
  if (seriesSet.empty())
  {
    ev->accept();
    return;
  }

  auto const resButton = QMessageBox::question(this, windowTitle(), tr("Are you sure you want to quit?\n"),
                                               QMessageBox::Cancel | QMessageBox::Yes,
                                               QMessageBox::Yes);
  if (resButton != QMessageBox::Yes)
    ev->ignore();
  else
    ev->accept();
}

void MainWindow::loadPhotos()
{
  LOG(TRACE) << "Loading more photos";
  importwiz::ImportWizard importWizard(this);

  if (importWizard.exec())
    seriesSet.addSeries(importWizard.selectedSeries());
  else
    LOG(TRACE) << "Loading canceled";
}

namespace {
struct ActionConfigurator
{
  ActionConfigurator(MainToolbar &toolbar) : toolbar(toolbar)
  {}

  template<typename Object, typename Method>
  void operator()(std::string const& configName, Object *object, Method && method)
  {
    widgets::toolbar::Signal const* buttonSig = toolbar.getSignal(configName);
    if (buttonSig)
      QObject::connect(buttonSig, &widgets::toolbar::Signal::activated, object, method);
  }

private:
  MainToolbar &toolbar;
};
} // unnamed namespace

void MainWindow::connectToolbar()
{
  ActionConfigurator conf(*mainToolbar);

// TODO: save option, with possibility to save scaled pixmaps as well, with metrics etc
// TODO: Load saved config from file, initialize all series, pixmaps, metrics, selections etc, remember to fix UUIDs for series as those will change (or maybe can construct QUuid back from text?
  conf("fileImport", this, &MainWindow::loadPhotos);
// TODO" configure Ctrl+Q event!
//(QKeySequence("Ctrl+Q"), tr("Exit the application"), "", this, &MainWindow::close);

// TODO: to viewMenubar add selectable options to enable/disable addons on photoitemwidgets
// TODO: Action: Report -> show dialog with number of series / num selected photos, num unchecked series etc
  conf("viewAllSeries", this, [this](){ viewStack->handleSwitchView(ViewDescription::switchTo(ViewType::ALL_SERIES)); });
  conf("viewSingleSeries", this, [this](){ viewStack->handleSwitchView(ViewDescription::switchTo(ViewType::NUM_SINGLE_SERIES)); });
  conf("viewScrollable", this, [this](){ viewStack->handleSwitchView(ViewDescription::switchTo(ViewType::ROW_SINGLE_SERIES)); });

  conf("viewLaboratory", this, [this](){ viewStack->handleSwitchView(ViewDescription::switchTo(ViewType::LABORATORY)); });
  conf("viewFullscreenPreview", this, &MainWindow::openFullscreenDialog);
  conf("viewPhotoDetails", this, &MainWindow::openDetailsDialog);

  conf("seriesNext", this, [this](){ viewStack->handleSwitchView(ViewDescription::moveNextSeries()); });
  conf("seriesPrevious", this, [this](){ viewStack->handleSwitchView(ViewDescription::movePreviousSeries()); });

// TODO: Select only best phtoos to clear selection and select best photos
  conf("selectBest", this, [this](){ viewStack->bulkSelect(PhotoBulkAction::SELECT_BEST); });
  conf("selectAll", this, [this](){ viewStack->bulkSelect(PhotoBulkAction::SELECT_ALL); });
  conf("selectInvert", this, [this](){ viewStack->bulkSelect(PhotoBulkAction::INVERT); });
  conf("selectClear", this, [this](){ viewStack->bulkSelect(PhotoBulkAction::CLEAR); });

// TODO: By default an action should call only the basic dialog window with only confirmation because usual actions are very simple and limited.
// Add another menuitem with Advanced usage which will open the current more complicated dialog
  conf("processDelete", this, [this]{ processAction(processwiz::OperationType::Delete); });
  conf("processMove", this, [this]{ processAction(processwiz::OperationType::Move); });
  conf("processCopy", this, [this]{ processAction(processwiz::OperationType::Copy); });
  conf("processRename", this, [this]{ processAction(processwiz::OperationType::Rename); });

  conf("enhanceWhiteBalance", this, [this]{ emit viewStack->photoEnhancement(iprocess::enhance::OperationType::AutoWhiteBalance); });
  conf("enhanceSave.save", this, &MainWindow::handleEnhanceSave);
  conf("enhanceSave.saveAs", this, &MainWindow::handleEnhanceSaveAs);
}

void MainWindow::configureStatusBar()
{
  statusBar()->addPermanentWidget(sharedWidgets.slider);
  statusBar()->addPermanentWidget(sharedWidgets.leftRightNav);

  sharedWidgets.slider->hide();
  sharedWidgets.leftRightNav->hide();
}

void MainWindow::processAction(processwiz::OperationType const operation)
{
  processwiz::ProcessWizard processWizard(this, seriesSet, operation);

  if (processWizard.exec())
    processwiz::exec::execute(seriesSet, processWizard.executions());
}

void MainWindow::openDetailsDialog()
{
  LOG(TRACE) << "Requested details dialog for focused item";
  auto const focusedItem = utils::focusedPhotoItemWidget();
  if (!focusedItem)
  {
    LOG(TRACE) << "No photo is focused";
    return;
  }

  focusedItem->openDetailsDialog();
}

// TODO: Disable buttons for fullscreen and details when there is no photos loaded and those functions fail
void MainWindow::openFullscreenDialog()
{
  LOG(TRACE) << "Requested fullscreen dialog for focused item";
  auto photoItem = utils::focusedPhotoItemWidget();
  if (!photoItem)
  {
// TODO: Find first possible item?
  }
  if (!photoItem)
  {
    LOG(TRACE) << "Unable to show fullscreen dialog";
    return;
  }

  photoItem->showInFullDialog();
}

void MainWindow::handleEnhanceSave()
{
  LOG(TRACE) << "Requested save in laboratory";
  auto const processedPhoto = viewStack->currentItemInLaboratory();
  if (!processedPhoto)
  {
    LOG(DEBUG) << "There is no currently processes photo in laboratory";
    return;
  }

  QString result = enhanceFilenameChooser->confirm(processedPhoto->fileName);
  if (result.isEmpty())
    return;

  viewStack->saveItemInLaboratory(result);
}

void MainWindow::handleEnhanceSaveAs()
{
  LOG(TRACE) << "Requested save as in laboratory";
  auto const processedPhoto = viewStack->currentItemInLaboratory();
  if (!processedPhoto)
  {
    LOG(DEBUG) << "There is no currently processes photo in laboratory";
    return;
  }

  QString result = enhanceFilenameChooser->select(processedPhoto->fileName);
  if (result.isEmpty())
    return;

  viewStack->saveItemInLaboratory(result);
}

// TODO: Status bar should display percent and fraction of photos(series) viewed, especially in series view

} // namespace phobos
