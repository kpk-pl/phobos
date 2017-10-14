#include "MainWindow.h"
#include "ViewDescription.h"
#include "ImageProcessing/Metrics.h"
#include "PhotoContainers/ItemId.h"
#include "Config.h"
#include "PhotoBulkAction.h"
#include "ImportWizard/Types.h"
#include "ProcessWizard/Action.h"
#include "ProcessWizard/Execution/Execution.h"
#include "PhotoContainers/ItemId.h"
#include <easylogging++.h>
#include <QApplication>
#include <QMetaType>
#include <QIcon>
#include <memory>

INITIALIZE_EASYLOGGINGPP

// TODO: My app has no icon - add one
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
    qRegisterMetaType<std::size_t>("std::size_t");
    qRegisterMetaType<phobos::ViewDescriptionPtr>();
    qRegisterMetaType<phobos::iprocess::metric::MetricPtr>();
    qRegisterMetaType<phobos::PhotoBulkAction>();
    qRegisterMetaType<phobos::importwiz::PhotoSeriesVec>();
    qRegisterMetaType<phobos::processwiz::ConstActionPtr>();
    qRegisterMetaType<phobos::processwiz::ConstActionPtrVec>();
    qRegisterMetaType<phobos::processwiz::ConstExecutionPtrVec>();
    qRegisterMetaType<phobos::pcontainer::ItemId>();

    phobos::MainWindow w;
    LOG(INFO) << "Application starting now...";
    w.show();

    return a.exec();
}
