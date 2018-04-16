#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "PhotoContainers/Set.h"
#include "ImageCache/Cache.h"
#include "ProcessWizard/Operation.h"
#include "SharedWidgets.h"

class QCloseEvent;

namespace phobos {

class ViewStack;
class MainToolbar;
namespace utils {
class FilenameChooser;
} // namespace utils

// TODO: https://stackoverflow.com/questions/4795757/is-there-a-better-way-to-wordwrap-text-in-qtooltip-than-just-using-regexp
// use last answer and install global filter

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);

  void closeEvent(QCloseEvent *ev) override;

private slots:
  void processAction(processwiz::OperationType const operation);
  void openDetailsDialog();
  void openFullscreenDialog();
  void handleEnhanceSaveAs();

private:
  void loadPhotos();

  void connectToolbar();
  void configureStatusBar();
  void connectNavigations();

  pcontainer::Set seriesSet;
  icache::Cache imageCache;

  SharedWidgets const sharedWidgets;

  MainToolbar* mainToolbar;
  ViewStack* viewStack;

  utils::FilenameChooser* enhanceFilenameChooser;
};

} // namespace phobos

#endif // MAINWINDOW_H
