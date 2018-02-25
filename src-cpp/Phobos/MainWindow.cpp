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
#include "Utils/Focused.h"
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
  viewStack(new ViewStack(seriesSet, imageCache, sharedWidgets, mainToolbar))
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
  importwiz::ImportWizard importWizard(this);
  if (importWizard.exec())
    seriesSet.addSeries(importWizard.selectedSeries());
}

namespace {
struct ActionConfigurator
{
  ActionConfigurator(MainToolbar &toolbar) : toolbar(toolbar)
  {}

  template<typename Object, typename Method>
  void operator()(QKeySequence const& shortcut,
                  QString const& tooltip,
                  std::string const& configName,
                  Object *object,
                  Method && method)
  {
    QToolButton *button = toolbar.getButton(configName);
    if (button)
    {
      if (shortcut.isEmpty())
      {
        button->setToolTip(tooltip);
      }
      else
      {
        button->setShortcut(shortcut);
        button->setToolTip(tooltip + " (<b>" + shortcut.toString() + "</b>)");
      }

      QObject::connect(button, &QToolButton::clicked, object, method);
    }
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
  conf(QKeySequence("Ctrl+O"), tr("Import photos"), "fileImport", this, &MainWindow::loadPhotos);
  conf(QKeySequence("Ctrl+Q"), tr("Exit the application"), "", this, &MainWindow::close);

// TODO: to viewMenubar add selectable options to enable/disable addons on photoitemwidgets
// TODO: Action: Report -> show dialog with number of series / num selected photos, num unchecked series etc
  conf(QKeySequence("Alt+1"), tr("Show all series on one page"), "viewAllSeries", this,
       [this](){ viewStack->handleSwitchView(ViewDescription::switchTo(ViewType::ALL_SERIES)); });
  conf(QKeySequence("Alt+2"), tr("Show side by side photos from one series"), "viewSingleSeries", this,
       [this](){ viewStack->handleSwitchView(ViewDescription::switchTo(ViewType::NUM_SINGLE_SERIES)); });
  conf(QKeySequence("Alt+3"), tr("Show one series with zoomed photos on a single page with horizontal scrolling capability"), "viewScrollable", this,
       [this](){ viewStack->handleSwitchView(ViewDescription::switchTo(ViewType::ROW_SINGLE_SERIES)); });

  conf(QKeySequence("Alt+4"), tr("Switch to enhancements and editing workspace"), "viewLaboratory", this,
       [this](){ viewStack->handleSwitchView(ViewDescription::switchTo(ViewType::LABORATORY)); });
  conf(QKeySequence("Shift+F"), tr("Open a separate preview dialog with a single fullscreen photo"), "viewFullscreenPreview", this, &MainWindow::openFullscreenDialog);
  conf(QKeySequence("Shift+D"), tr("Show details for selected photo"), "viewPhotoDetails", this, &MainWindow::openDetailsDialog);

  conf(QKeySequence("Shift+Right"), tr("Jump to next series"), "seriesNext", this,
       [this](){ viewStack->handleSwitchView(ViewDescription::moveNextSeries()); });
  conf(QKeySequence("Shift+Left"), tr("Jump to previous series"), "seriesPrevious", this,
       [this](){ viewStack->handleSwitchView(ViewDescription::movePreviousSeries()); });

// TODO: Select only best phtoos to clear selection and select best photos
  conf(QKeySequence(), tr("Automatically select best photos in each series"), "selectBest", this,
       [this](){ viewStack->bulkSelect(PhotoBulkAction::SELECT_BEST); });
  conf(QKeySequence(), tr("Select all photos"), "selectAll", this,
       [this](){ viewStack->bulkSelect(PhotoBulkAction::SELECT_ALL); });
  conf(QKeySequence(), tr("Invert selection"), "selectInvert", this,
       [this](){ viewStack->bulkSelect(PhotoBulkAction::INVERT); });
  conf(QKeySequence(), tr("Clear selection"), "selectClear", this,
       [this](){ viewStack->bulkSelect(PhotoBulkAction::CLEAR); });

// TODO: By default an action should call only the basic dialog window with only confirmation because usual actions are very simple and limited.
// Add another menuitem with Advanced usage which will open the current more complicated dialog
  conf(QKeySequence(), tr("Delete selected files from hard drive"), "processDelete", this,
       [this]{ processAction(processwiz::OperationType::Delete); });
  conf(QKeySequence(), tr("Move selected files from hard drive"), "processMove", this,
       [this]{ processAction(processwiz::OperationType::Move); });
  conf(QKeySequence(), tr("Copy selected files from hard drive"), "processCopy", this,
       [this]{ processAction(processwiz::OperationType::Copy); });
  conf(QKeySequence(), tr("Rename selected files from hard drive"), "processRename", this,
       [this]{ processAction(processwiz::OperationType::Rename); });
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
  LOG(INFO) << "Requested details dialog for focused item";
  auto const focusedItem = utils::focusedPhotoItemWidget();
  if (!focusedItem)
  {
    LOG(INFO) << "No photo is focused";
    return;
  }

  focusedItem->openDetailsDialog();
}

// TODO: Disable buttons for fullscreen and details when there is no photos loaded and those functions fail
void MainWindow::openFullscreenDialog()
{
  LOG(INFO) << "Requested fullscreen dialog for focused item";
  auto photoItem = utils::focusedPhotoItemWidget();
  if (!photoItem)
  {
// TODO: Find first possible item?
  }
  if (!photoItem)
  {
    LOG(INFO) << "Unable to show fullscreen dialog";
    return;
  }

  photoItem->showInFullDialog();
}

// TODO: Status bar should display percent and fraction of photos(series) viewed, especially in series view

} // namespace phobos
