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
// TODO: Need to add a lot of helper tests to be displayed in the status bar of the application
// Possibility to disable status bar entirely
// Every screen and page should say what is expected of the user
// and if mouse toogles over some UI element text should shange to a description what this element does

int main(int argc, char *argv[])
{
    auto const elConf = phobos::config::get()->get_qualified_as<std::string>("logging.config");
    if (elConf)
    {
        el::Configurations conf(*elConf);
        el::Loggers::reconfigureAllLoggers(conf);
    }

    LOG(DEBUG) << "Logger configured";

    QApplication a(argc, argv);
    qRegisterMetaType<std::size_t>("std::size_t");
    qRegisterMetaType<phobos::ViewDescriptionPtr>();
    qRegisterMetaType<phobos::iprocess::MetricPtr>();
    qRegisterMetaType<phobos::PhotoBulkAction>();
    qRegisterMetaType<phobos::importwiz::PhotoSeriesVec>();
    qRegisterMetaType<phobos::processwiz::ConstActionPtr>();
    qRegisterMetaType<phobos::processwiz::ConstActionPtrVec>();
    qRegisterMetaType<phobos::processwiz::ConstExecutionPtrVec>();
    qRegisterMetaType<phobos::pcontainer::ItemId>();

    phobos::MainWindow w;
    LOG(TRACE) << "Application starting now...";
    w.show();

    return a.exec();
}
