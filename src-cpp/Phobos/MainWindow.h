#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ViewStack.h"
#include "PhotoContainers/Set.h"
#include "ImageCache/Cache.h"
#include "ProcessWizard/Operation.h"

class QCloseEvent;

namespace phobos {

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);

  void closeEvent(QCloseEvent *ev) override;

private slots:
  void processAction(processwiz::OperationType const operation);
  void openDetailsDialog();

private:
  void loadPhotos();
  void createMenus();
  void connectNavigations();

  pcontainer::Set seriesSet;
  icache::Cache imageCache;
  ViewStack* viewStack;
};

} // namespace phobos

#endif // MAINWINDOW_H
