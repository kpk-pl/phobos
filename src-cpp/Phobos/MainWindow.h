#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ViewStack.h"
#include "PhotoContainers/Set.h"
#include "ImageCache/Cache.h"

namespace phobos {

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void removeSelected() const;
    void moveSelected() const;
    void copySelected() const;

private:
    void loadPhotos();
    void createMenus();

    pcontainer::Set seriesSet;
    icache::Cache imageCache;
    ViewStack* viewStack;
};

} // namespace phobos

#endif // MAINWINDOW_H
