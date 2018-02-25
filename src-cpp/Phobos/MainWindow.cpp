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
#include "ImageProcessing/Utils/ColoredPixmap.h"
#include "Widgets/StatusBarSlider.h"
#include "Widgets/StatusBarLeftRightNavigation.h"
#include "Utils/Focused.h"
#include <easylogging++.h>
#include <QApplication>
#include <QMenuBar>
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
  viewStack(new ViewStack(seriesSet, imageCache, sharedWidgets)),
  mainToolbar(config::qualified("mainWindow.enableToolbar", true) ? new MainToolbar : nullptr)
{
  if (mainToolbar)
  {
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(mainToolbar);
    mainLayout->addWidget(viewStack);

    QWidget *mainWidget = new QWidget;
    mainWidget->setLayout(mainLayout);
    setCentralWidget(mainWidget);
  }
  else
  {
    setCentralWidget(viewStack);
  }

  createMenus();
  if (!config::qualified("mainWindow.enableMenu", false))
    menuBar()->hide();

  configureStatusBar();
  connectNavigations();

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

void MainWindow::createMenus()
{
  createFileMenu();
  createViewMenu();
  createSelectMenu();
  createProcessMenu();
  createHelpMenu();
}

namespace {
QPixmap quickIcon(std::string const& configName)
{
  return iprocess::utils::coloredPixmap(config::ConfigPath("navigationBar")(configName), QSize(64, 64));
}

struct ActionConfigurator
{
  ActionConfigurator(QMenu *menu, MainToolbar *toolbar = nullptr) : menu(menu), toolbar(toolbar)
  {}

  template<typename Object, typename Method>
  void operator()(QString const& menuName,
                  QKeySequence const& shortcut,
                  QString const& tooltip,
                  std::string const& configName,
                  Object *object,
                  Method && method)
  {
    if (configName.empty())
      menu->addAction(menuName, object, method, shortcut)->setStatusTip(tooltip);
    else
      menu->addAction(quickIcon(configName), menuName, object, method, shortcut)->setStatusTip(tooltip);

    if (toolbar && !configName.empty())
    {
      QToolButton *button = toolbar->getButton(configName);
      if (button)
      {
        button->setToolTip(tooltip);
        button->setShortcut(shortcut);
        QObject::connect(button, &QToolButton::clicked, object, method);
      }
    }
  }

private:
  QMenu *menu;
  MainToolbar *toolbar;
};

} // unnamed namespace

// TODO: save option, with possibility to save scaled pixmaps as well, with metrics etc
// TODO: Load saved config from file, initialize all series, pixmaps, metrics, selections etc, remember to fix UUIDs for series as those will change (or maybe can construct QUuid back from text?
void MainWindow::createFileMenu()
{
  QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
  ActionConfigurator conf(fileMenu, mainToolbar);

  conf(tr("&Open"), QKeySequence("Ctrl+O"), tr("Import photos"), "fileImport", this, &MainWindow::loadPhotos);
  fileMenu->addSeparator();

  conf(tr("&Exit"), QKeySequence("Ctrl+Q"), tr("Exit the application"), "", this, &MainWindow::close);
}

// TODO: to viewMenubar add selectable options to enable/disable addons on photoitemwidgets
// TODO: Select only best phtoos to clear selection and select best photos

// TODO: Add shortcut representation to tooltips!
void MainWindow::createViewMenu()
{
  QMenu* viewMenu = menuBar()->addMenu(tr("&View"));
  ActionConfigurator conf(viewMenu, mainToolbar);

  conf(tr("&All series"), QKeySequence("Alt+1"), tr("Show all series on one page"), "viewAllSeries", this,
       [this](){ viewStack->handleSwitchView(ViewDescription::switchTo(ViewType::ALL_SERIES)); });
  conf(tr("&Single series"), QKeySequence("Alt+2"), tr("Show side by side photos from one series"), "viewSingleSeries", this,
       [this](){ viewStack->handleSwitchView(ViewDescription::switchTo(ViewType::NUM_SINGLE_SERIES)); });
  conf(tr("S&crollable"), QKeySequence("Alt+3"), tr("Show one series with zoomed photos on a single page with horizontal scrolling capability"), "viewScrollable", this,
       [this](){ viewStack->handleSwitchView(ViewDescription::switchTo(ViewType::ROW_SINGLE_SERIES)); });
  viewMenu->addSeparator();

  conf(tr("&Enhance photos"), QKeySequence("Alt+4"), tr("Switch to enhancements and editing workspace"), "viewLaboratory", this,
       [this](){ viewStack->handleSwitchView(ViewDescription::switchTo(ViewType::LABORATORY)); });
  conf(tr("&Fullscreen preview"), QKeySequence("Shift+F"), tr("Open a separate preview dialog with a single fullscreen photo"), "viewFullscreenPreview", this, &MainWindow::openFullscreenDialog);
  conf(tr("&Details"), QKeySequence("Shift+D"), tr("Show details for selected photo"), "viewPhotoDetails", this, &MainWindow::openDetailsDialog);
  viewMenu->addSeparator();

  conf(tr("&Next series"), QKeySequence("Shift+Right"), tr("Jump to next series"), "seriesNext", this,
       [this](){ viewStack->handleSwitchView(ViewDescription::moveNextSeries()); });
  conf(tr("&Previous series"), QKeySequence("Shift+Left"), tr("Jump to previous series"), "seriesPrevious", this,
       [this](){ viewStack->handleSwitchView(ViewDescription::movePreviousSeries()); });
}

// TODO: Action: Report -> show dialog with number of series / num selected photos, num unchecked series etc
void MainWindow::createSelectMenu()
{
  QMenu* selectMenu = menuBar()->addMenu(tr("&Select"));
  ActionConfigurator conf(selectMenu, mainToolbar);

  conf(tr("Select &best"), QKeySequence(), tr("Automatically select best photos in each series"), "selectBest", this,
       [this](){ viewStack->bulkSelect(PhotoBulkAction::SELECT_BEST); });
  conf(tr("Select &all"), QKeySequence(), tr("Select all photos"), "selectAll", this,
       [this](){ viewStack->bulkSelect(PhotoBulkAction::SELECT_ALL); });
  conf(tr("&Invert selection"), QKeySequence(), tr("Invert selection"), "selectInvert", this,
       [this](){ viewStack->bulkSelect(PhotoBulkAction::INVERT); });
  conf(tr("&Clear selection"), QKeySequence(), tr("Clear selection"), "selectClear", this,
       [this](){ viewStack->bulkSelect(PhotoBulkAction::CLEAR); });
}

// TODO: By default an action should call only the basic dialog window with only confirmation because usual actions are very simple and limited.
// Add another menuitem with Advanced usage which will open the current more complicated dialog
void MainWindow::createProcessMenu()
{
  QMenu* execMenu = menuBar()->addMenu(tr("&Process"));
  ActionConfigurator conf(execMenu, mainToolbar);

  conf(tr("&Delete"), QKeySequence(), tr("Delete selected files from hard drive"), "processDelete", this,
       [this]{ processAction(processwiz::OperationType::Delete); });
  conf(tr("&Move"), QKeySequence(), tr("Move selected files from hard drive"), "processMove", this,
       [this]{ processAction(processwiz::OperationType::Move); });
  conf(tr("&Copy"), QKeySequence(), tr("Copy selected files from hard drive"), "processCopy", this,
       [this]{ processAction(processwiz::OperationType::Copy); });
  conf(tr("&Rename"), QKeySequence(), tr("Rename selected files from hard drive"), "processRename", this,
       [this]{ processAction(processwiz::OperationType::Rename); });
}

void MainWindow::createHelpMenu()
{
  QMenu* helpMenu = menuBar()->addMenu(tr("Help"));
  ActionConfigurator conf(helpMenu, mainToolbar);

//  conf(tr("Credits"), QKeySequence(), tr("Show licensing and credits information"), "helpLicense", this, ???)
//  helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
}

void MainWindow::configureStatusBar()
{
  statusBar()->addPermanentWidget(sharedWidgets.slider);
  statusBar()->addPermanentWidget(sharedWidgets.leftRightNav);

  sharedWidgets.slider->hide();
  sharedWidgets.leftRightNav->hide();
}

void MainWindow::connectNavigations()
{
  QObject::connect(viewStack, &ViewStack::importPhotosRequest, this, &MainWindow::loadPhotos);
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
