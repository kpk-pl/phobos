#include <memory>
#include <QApplication>
#include <QPixmap>
#include "MainWindow.h"
#include "ViewDescription.h"

Q_DECLARE_METATYPE(std::shared_ptr<QPixmap>)
Q_DECLARE_METATYPE(phobos::ViewDescriptionPtr)

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<std::shared_ptr<QPixmap> >();
    qRegisterMetaType<phobos::ViewDescriptionPtr>();

    phobos::MainWindow w;
    w.show();

    return a.exec();
}
