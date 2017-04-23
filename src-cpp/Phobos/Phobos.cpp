#include <memory>
#include <QApplication>
#include <QPixmap>
#include <easylogging++.h>
#include "MainWindow.h"
#include "ViewDescription.h"
#include "ImageProcessing/Metrics.h"
#include "Config.h"
#include "PhotoBulkAction.h"
#include "ImportWizard/Types.h"

INITIALIZE_EASYLOGGINGPP

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
    qRegisterMetaType<phobos::ViewDescriptionPtr>();
    qRegisterMetaType<phobos::iprocess::MetricPtr>();
    qRegisterMetaType<phobos::PhotoBulkAction>();
    qRegisterMetaType<phobos::importwiz::PhotoSeriesVec>();

    phobos::MainWindow w;
    LOG(INFO) << "Application starting now...";
    w.show();

    return a.exec();
}
