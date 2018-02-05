QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Phobos
TEMPLATE = app
CONFIG += c++14
RESOURCES = resources.qrc

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += ELPP_THREAD_SAFE
DEFINES += ELPP_FEATURE_CRASH_LOG
DEFINES += ELPP_STL_LOGGING
DEFINES += ELPP_QT_LOGGING
DEFINES += ELPP_FEATURE_PERFORMANCE_TRACKING

QMAKE_CXXFLAGS += -Wno-unused-variable -Wuninitialized -Winit-self

SOURCES += Phobos.cpp\
    MainWindow.cpp \
    ViewStack.cpp \
    Config.cpp \
    ConfigPath.cpp \
    AllSeriesView.cpp \
    WelcomeView.cpp \
    ConfigExtension.cpp \
    SeriesViewBase.cpp \
    NumSeriesView.cpp \
    RowSeriesView.cpp \
    ImageProcessing/Metrics.cpp \
    ImageProcessing/MetricsIO.cpp \
    ImageProcessing/LoaderThread.cpp \
    ImageProcessing/Calculator/Bluriness.cpp \
    ImageProcessing/Calculator/Hue.cpp \
    ImageProcessing/Calculator/Sharpness.cpp \
    ImageProcessing/Calculator/Noisiness.cpp \
    ImageProcessing/Calculator/Histogram.cpp \
    ImageProcessing/Calculator/Saturation.cpp \
    ImageProcessing/MetricsAggregate.cpp \
    ImageProcessing/MetricCalculator.cpp \
    ImageProcessing/Utils/ColoredPixmap.cpp \
    ImageProcessing/Utils/ScalePixmap.cpp \
    ImageProcessing/Utils/FormatConversion.cpp \
    PhotoContainers/Item.cpp \
    PhotoContainers/Series.cpp \
    PhotoContainers/Set.cpp \
    PhotoContainers/ItemState.cpp \
    PhotoContainers/FileInfo.cpp \
    Utils/LayoutClear.cpp \
    Utils/Asserted.cpp \
    Utils/ItemStateColor.cpp \
    Utils/ItemStateIcon.cpp \
    Utils/Filesystem/Portable.cpp \
    Utils/Filesystem/Attributes.cpp \
    Utils/Filesystem/Trash.cpp \
    easyloggingpp/src/easylogging++.cc \
    qt_ext/qexifimageheader.cpp \
    ImportWizard/ImageOpenDialog.cpp \
    ImportWizard/ImportWizard.cpp \
    ImportWizard/DivisionMethodPage.cpp \
    ImportWizard/DivisionOps.cpp \
    ImportWizard/SeriesDisplayPage.cpp \
    ImportWizard/FileInfoProvider.cpp \
    Widgets/IconLabel.cpp \
    Widgets/NavigationBar.cpp \
    Widgets/HorizontalScrollArea.cpp \
    Widgets/ImageWidget.cpp \
    Widgets/PhotoItem/PhotoItem.cpp \
    Widgets/PhotoItem/Addon.cpp \
    Widgets/PhotoItem/Recovery.cpp \
    Widgets/PhotoItem/DetailsDialog.cpp \
    Widgets/PhotoItem/AddonRenderer.cpp \
    Widgets/FilenameEntry.cpp \
    Widgets/ClickableLabel.cpp \
    ImageCache/Cache.cpp \
    ImageCache/MetricCache.cpp \
    ImageCache/LimitedMap.cpp \
    ImageCache/PriorityThreadPool.cpp \
    ImageCache/Transaction.cpp \
    ImageCache/Runnable.cpp \
    ImageCache/ContentList.cpp \
    ImageCache/LoadingManager.cpp \
    ImageCache/ProactiveScheduler.cpp \
    ProcessWizard/ProcessWizard.cpp \
    ProcessWizard/Operation.cpp \
    ProcessWizard/ActionsCreatorPage.cpp \
    ProcessWizard/Action.cpp \
    ProcessWizard/SeriesCounts.cpp \
    ProcessWizard/WarningsPage.cpp \
    ProcessWizard/TypeActionTab.cpp \
    ProcessWizard/SummaryPage.cpp \
    ProcessWizard/OperationIcon.cpp \
    ProcessWizard/ActionsCreatorResources.cpp \
    ProcessWizard/ActionTab/ActionTab.cpp \
    ProcessWizard/ActionTab/DeleteActionTab.cpp \
    ProcessWizard/ActionTab/RenameActionTab.cpp \
    ProcessWizard/ActionTab/CopyMoveActionTab.cpp \
    ProcessWizard/Execution/ExecutionImpl.cpp \
    ProcessWizard/Execution/Execute.cpp \
    Widgets/HVLine.cpp

HEADERS  += MainWindow.h \
    ViewStack.h \
    Config.h \
    ConfigPath.h \
    AllSeriesView.h \
    WelcomeView.h \
    ConfigExtension.h \
    SeriesViewBase.h \
    NumSeriesView.h \
    RowSeriesView.h \
    ViewDescription.h \
    PhotoBulkAction.h \
    ImageProcessing/MetricsFwd.h \
    ImageProcessing/Metrics.h \
    ImageProcessing/MetricsIO.h \
    ImageProcessing/LoaderThread.h \
    ImageProcessing/Calculator/All.h \
    ImageProcessing/Calculator/Bluriness.h \
    ImageProcessing/Calculator/Hue.h \
    ImageProcessing/Calculator/Sharpness.h \
    ImageProcessing/Calculator/Noisiness.h \
    ImageProcessing/Calculator/Histogram.h \
    ImageProcessing/Calculator/Saturation.h \
    ImageProcessing/MetricsAggregate.h \
    ImageProcessing/MetricCalculator.h \
    ImageProcessing/Metric/Traits.h \
    ImageProcessing/Metric/MetricType.h \
    ImageProcessing/Metric/Blur.h \
    ImageProcessing/Metric/All.h \
    ImageProcessing/Metric/Contrast.h \
    ImageProcessing/Metric/Noise.h \
    ImageProcessing/Metric/Sharpness.h \
    ImageProcessing/Metric/DepthOfField.h \
    ImageProcessing/Metric/Saturation.h \
    ImageProcessing/Metric/ComplementaryColors.h \
    ImageProcessing/Feature/Histogram.h \
    ImageProcessing/Feature/Hue.h \
    ImageProcessing/Feature/All.h \
    ImageProcessing/Utils/ColoredPixmap.h \
    ImageProcessing/Utils/ROIProcessor.h \
    ImageProcessing/Utils/FormatConversion.h \
    ImageProcessing/Utils/ScalePixmap.h \
    PhotoContainers/Fwd.h \
    PhotoContainers/Series.h \
    PhotoContainers/Set.h \
    PhotoContainers/Item.h \
    PhotoContainers/ItemId.h \
    PhotoContainers/ItemState.h \
    PhotoContainers/FileInfo.h \
    Utils/LayoutClear.h \
    Utils/Algorithm.h \
    Utils/Focused.h \
    Utils/Asserted.h \
    Utils/Comparators.h \
    Utils/Invoke.h \
    Utils/Streaming.h \
    Utils/LexicalCast.h \
    Utils/ItemStateColor.h \
    Utils/ItemStateIcon.h \
    Utils/Soul.h \
    Utils/Filesystem/Attributes.h \
    Utils/Filesystem/Portable.h \
    Utils/Filesystem/Trash.h \
    Utils/Circulator.h \
    easyloggingpp/src/easylogging++.h \
    qt_ext/qexifimageheader.h \
    ImportWizard/ImageOpenDialog.h \
    ImportWizard/ImportWizard.h \
    ImportWizard/DivisionMethodPage.h \
    ImportWizard/DivisionOps.h \
    ImportWizard/Types.h \
    ImportWizard/SeriesDisplayPage.h \
    ImportWizard/FileInfoProvider.h \
    Widgets/HeightResizeableInterface.h \
    Widgets/IconLabel.h \
    Widgets/NavigationBar.h \
    Widgets/HorizontalScrollArea.h \
    Widgets/ImageWidget.h \
    Widgets/PhotoItem/PhotoItem.h \
    Widgets/PhotoItem/Addon.h \
    Widgets/PhotoItem/Capability.h \
    Widgets/PhotoItem/Recovery.h \
    Widgets/PhotoItem/DetailsDialog.h \
    Widgets/PhotoItem/AddonRenderer.h \
    Widgets/FilenameEntry.h \
    Widgets/ClickableLabel.h \
    ImageCache/Cache.h \
    ImageCache/MetricCache.h \
    ImageCache/CacheFwd.h \
    ImageCache/LimitedMap.h \
    ImageCache/PriorityThreadPool.h \
    ImageCache/Transaction.h \
    ImageCache/TransactionFwd.h \
    ImageCache/Runnable.h \
    ImageCache/ContentList.h \
    ImageCache/Types.h \
    ImageCache/LoadingManager.h \
    ImageCache/LoadingJob.h \
    ImageCache/ProactiveScheduler.h \
    ProcessWizard/ProcessWizard.h \
    ProcessWizard/Operation.h \
    ProcessWizard/ActionsCreatorPage.h \
    ProcessWizard/Action.h \
    ProcessWizard/ActionFwd.h \
    ProcessWizard/SeriesCounts.h \
    ProcessWizard/WarningsPage.h \
    ProcessWizard/TypeActionTab.h \
    ProcessWizard/SummaryPage.h \
    ProcessWizard/OperationIcon.h \
    ProcessWizard/ActionsCreatorResources.h \
    ProcessWizard/ActionTab/ActionTab.h \
    ProcessWizard/ActionTab/DeleteActionTab.h \
    ProcessWizard/ActionTab/RenameActionTab.h \
    ProcessWizard/ActionTab/CopyMoveActionTab.h \
    ProcessWizard/Execution/Execution.h \
    ProcessWizard/Execution/ExecutionFwd.h \
    ProcessWizard/Execution/ExecutionImpl.h \
    ProcessWizard/Execution/Execute.h \
    Widgets/HVLine.h \
    Utils/PainterFrame.h

INCLUDEPATH += cpptoml
INCLUDEPATH += boost
INCLUDEPATH += opencv/include
INCLUDEPATH += easyloggingpp/src

win32 {
LIBS += $$PWD/opencv/bin/libopencv_core320.dll
LIBS += $$PWD/opencv/bin/libopencv_imgcodecs320.dll
LIBS += $$PWD/opencv/bin/libopencv_imgproc320.dll
} else {
LIBS += `pkg-config opencv --libs`
}

CONFIG(release, debug|release): DESTDIR = $$OUT_PWD/release
CONFIG(debug, debug|release): DESTDIR = $$OUT_PWD/debug

copydata_config.commands = $(COPY_FILE) \"$$shell_path($$PWD\\config.toml)\" \"$$shell_path($$DESTDIR)\"
copydata_logconfig.commands = $(COPY_FILE) \"$$shell_path($$PWD\\logging.conf)\" \"$$shell_path($$DESTDIR)\"
first.depends = $(first) copydata_config copydata_logconfig
export(first.depends)
export(copydata_config.commands)
export(copydata_logconfig.commands)
QMAKE_EXTRA_TARGETS += first copydata_config copydata_logconfig
