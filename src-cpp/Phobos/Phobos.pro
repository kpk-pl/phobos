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

QMAKE_CXXFLAGS += -Wno-unused-variable

SOURCES += Phobos.cpp\
    MainWindow.cpp \
    ViewStack.cpp \
    Config.cpp \
    ConfigPath.cpp \
    AllSeriesView.cpp \
    ImageProcessing/Metrics.cpp \
    ImageProcessing/MetricsIO.cpp \
    ImageProcessing/LoaderThread.cpp \
    ImageProcessing/FormatConversion.cpp \
    ImageProcessing/Bluriness.cpp \
    ImageProcessing/Noisiness.cpp \
    ImageProcessing/Histogram.cpp \
    ImageProcessing/MetricsAggregate.cpp \
    ImageProcessing/ColoredPixmap.cpp \
    ImageProcessing/ScalePixmap.cpp \
    PhotoContainers/Item.cpp \
    PhotoContainers/Series.cpp \
    PhotoContainers/Set.cpp \
    PhotoContainers/ItemState.cpp \
    Utils/LayoutClear.cpp \
    Utils/ItemStateColor.cpp \
    Utils/ItemStateIcon.cpp \
    Utils/Filesystem/Portable.cpp \
    Utils/Filesystem/Attributes.cpp \
    Utils/Filesystem/Trash.cpp \
    easyloggingpp/src/easylogging++.cc \
    qt_ext/qexifimageheader.cpp \
    ConfigExtension.cpp \
    SeriesViewBase.cpp \
    NumSeriesView.cpp \
    NavigationBar.cpp \
    RowSeriesView.cpp \
    ImportWizard/ImageOpenDialog.cpp \
    ImportWizard/ImportWizard.cpp \
    ImportWizard/DivisionMethodPage.cpp \
    Widgets/IconLabel.cpp \
    Widgets/HorizontalScrollArea.cpp \
    Widgets/ImageWidget.cpp \
    Widgets/PhotoItem/PhotoItem.cpp \
    Widgets/PhotoItem/Addon.cpp \
    Widgets/HorizontalLine.cpp \
    Widgets/FilenameEntry.cpp \
    ImportWizard/DivisionOps.cpp \
    ImportWizard/SeriesDisplayPage.cpp \
    ImageCache/Cache.cpp \
    ImageCache/LimitedMap.cpp \
    ProcessWizard/ProcessWizard.cpp \
    ProcessWizard/Operation.cpp \
    ProcessWizard/ActionsCreatorPage.cpp \
    ProcessWizard/Action.cpp \
    ProcessWizard/ActionTab.cpp \
    ProcessWizard/SeriesCounts.cpp \
    ProcessWizard/WarningsPage.cpp \
    ProcessWizard/TypeActionTab.cpp \
    ProcessWizard/SummaryPage.cpp \
    ProcessWizard/OperationIcon.cpp \
    ProcessWizard/ExecutionImpl.cpp \
    PhotoContainers/ExifData.cpp

HEADERS  += MainWindow.h \
    ViewStack.h \
    Config.h \
    ConfigPath.h \
    AllSeriesView.h \
    ConfigExtension.h \
    ImageProcessing/Metrics.h \
    ImageProcessing/MetricsIO.h \
    ImageProcessing/LoaderThread.h \
    ImageProcessing/ScalePixmap.h \
    ImageProcessing/FormatConversion.h \
    ImageProcessing/Bluriness.h \
    ImageProcessing/Noisiness.h \
    ImageProcessing/Histogram.h \
    ImageProcessing/MetricsAggregate.h \
    ImageProcessing/ColoredPixmap.h \
    PhotoContainers/Fwd.h \
    PhotoContainers/Series.h \
    PhotoContainers/Set.h \
    PhotoContainers/Item.h \
    PhotoContainers/ItemId.h \
    PhotoContainers/ItemState.h \
    Utils/LayoutClear.h \
    Utils/Algorithm.h \
    Utils/Focused.h \
    Utils/Asserted.h \
    Utils/Comparators.h \
    Utils/Streaming.h \
    Utils/LexicalCast.h \
    Utils/ItemStateColor.h \
    Utils/ItemStateIcon.h \
    Utils/Filesystem/Attributes.h \
    Utils/Filesystem/Portable.h \
    Utils/Filesystem/Trash.h \
    easyloggingpp/src/easylogging++.h \
    qt_ext/qexifimageheader.h \
    SeriesViewBase.h \
    NumSeriesView.h \
    NavigationBar.h \
    RowSeriesView.h \
    ViewDescription.h \
    PhotoBulkAction.h \
    ImportWizard/ImageOpenDialog.h \
    ImportWizard/ImportWizard.h \
    ImportWizard/DivisionMethodPage.h \
    Widgets/HeightResizeableInterface.h \
    Widgets/IconLabel.h \
    Widgets/HorizontalScrollArea.h \
    Widgets/ImageWidget.h \
    Widgets/PhotoItem/PhotoItem.h \
    Widgets/PhotoItem/Addon.h \
    Widgets/PhotoItem/Capability.h \
    Widgets/HorizontalLine.h \
    Widgets/FilenameEntry.h \
    ImportWizard/DivisionOps.h \
    ImportWizard/Types.h \
    ImportWizard/SeriesDisplayPage.h \
    ImageCache/Cache.h \
    ImageCache/CacheFwd.h \
    ImageCache/LimitedMap.h \
    ProcessWizard/ProcessWizard.h \
    ProcessWizard/Operation.h \
    ProcessWizard/ActionsCreatorPage.h \
    ProcessWizard/Action.h \
    ProcessWizard/ActionFwd.h \
    ProcessWizard/ActionTab.h \
    ProcessWizard/SeriesCounts.h \
    ProcessWizard/WarningsPage.h \
    ProcessWizard/TypeActionTab.h \
    ProcessWizard/SummaryPage.h \
    ProcessWizard/OperationIcon.h \
    ProcessWizard/Execution.h \
    ProcessWizard/ExecutionFwd.h \
    ProcessWizard/ExecutionImpl.h \
    PhotoContainers/ExifData.h

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
