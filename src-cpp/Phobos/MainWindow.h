#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "PhotoContainers/Set.h"
#include "ImageCache/Cache.h"
#include "ProcessWizard/Operation.h"

class QCloseEvent;

namespace phobos {

class ViewStack;
class MainToolbar;

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

private:
  void loadPhotos();

  void createMenus();
  void createFileMenu();
  void createViewMenu();
  void createSelectMenu();
  void createProcessMenu();
  void createHelpMenu();

  void connectNavigations();

  pcontainer::Set seriesSet;
  icache::Cache imageCache;
  ViewStack* viewStack;
  MainToolbar* mainToolbar;
};

} // namespace phobos

#endif // MAINWINDOW_H
