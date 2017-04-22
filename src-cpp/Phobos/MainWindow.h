#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ViewStack.h"

namespace phobos {

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void loadPhotos();
    void createMenus();

    ViewStack* viewStack;
};

} // namespace phobos

#endif // MAINWINDOW_H
