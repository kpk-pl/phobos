#include <memory>
#include <QApplication>
#include <QPixmap>
#include <easylogging++.h>
#include "MainWindow.h"
#include "ViewDescription.h"
#include "ImageProcessing/Metrics.h"
#include "Config.h"

INITIALIZE_EASYLOGGINGPP

// TODO QPixmap uses implicit data sharing -> it is OK to pass it by value
Q_DECLARE_METATYPE(std::shared_ptr<QPixmap>)
Q_DECLARE_METATYPE(phobos::ViewDescriptionPtr)
Q_DECLARE_METATYPE(phobos::iprocess::MetricPtr)

int main(int argc, char *argv[])
{
    auto const elConf = phobos::config::get()->get_qualified_as<std::string>("global.loggingConfig");
    if (elConf)
    {
        el::Configurations conf(*elConf);
        el::Loggers::reconfigureAllLoggers(conf);
    }

    LOG(DEBUG) << "Logger configured";

    QApplication a(argc, argv);
    qRegisterMetaType<std::shared_ptr<QPixmap> >();
    qRegisterMetaType<phobos::ViewDescriptionPtr>();
    qRegisterMetaType<phobos::iprocess::MetricPtr>();

    phobos::MainWindow w;
    LOG(INFO) << "Application starting now...";
    w.show();

    return a.exec();
}
