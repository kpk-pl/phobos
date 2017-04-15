#include <memory>
#include <QApplication>
#include <QPixmap>
#include "MainWindow.h"
#include "ViewDescription.h"
#include "ImageProcessing/Metrics.h"

Q_DECLARE_METATYPE(std::shared_ptr<QPixmap>)
Q_DECLARE_METATYPE(phobos::ViewDescriptionPtr)
Q_DECLARE_METATYPE(phobos::iprocess::MetricPtr)

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<std::shared_ptr<QPixmap> >();
    qRegisterMetaType<phobos::ViewDescriptionPtr>();
    qRegisterMetaType<phobos::iprocess::MetricPtr>();

    phobos::MainWindow w;
    w.show();

    return a.exec();
}
