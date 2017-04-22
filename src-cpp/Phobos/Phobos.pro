QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Phobos
TEMPLATE = app
CONFIG += c++14

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
    AllSeriesView.cpp \
    ImageWidget.cpp \
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
    Utils/LayoutClear.cpp \
    easyloggingpp/src/easylogging++.cc \
    ConfigExtension.cpp \
    PhotoItemWidget.cpp \
    PhotoItemWidgetAddon.cpp \
    SeriesViewBase.cpp \
    NumSeriesView.cpp \
    NavigationBar.cpp \
    RowSeriesView.cpp \
    HorizontalScrollArea.cpp \
    ImportWizard/ImageOpenDialog.cpp \
    ImportWizard/ImportWizard.cpp \
    ImportWizard/DivisionMethodPage.cpp \
    Utils/FileAttributes.cpp \
    ImportWizard/DivisionOps.cpp

HEADERS  += MainWindow.h \
    ViewStack.h \
    Config.h \
    AllSeriesView.h \
    ConfigExtension.h \
    ImageWidget.h \
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
    PhotoContainers/Series.h \
    PhotoContainers/Set.h \
    PhotoContainers/Item.h \
    Utils/LayoutClear.h \
    Utils/Algorithm.h \
    Utils/Focused.h \
    easyloggingpp/src/easylogging++.h \
    PhotoItemWidget.h \
    PhotoItemWidgetAddon.h \
    SeriesViewBase.h \
    NumSeriesView.h \
    NavigationBar.h \
    RowSeriesView.h \
    HorizontalScrollArea.h \
    HeightResizeableInterface.h \
    ViewDescription.h \
    PhotoBulkAction.h \
    ImportWizard/ImageOpenDialog.h \
    ImportWizard/ImportWizard.h \
    ImportWizard/DivisionMethodPage.h \
    Utils/FileAttributes.h \
    ImportWizard/DivisionOps.h \
    ImportWizard/Types.h

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
copydata_icons.commands = $(COPY_DIR) \"$$shell_path($$PWD\\icon)\" \"$$shell_path($$DESTDIR\\icon)\"
copydata_logconfig.commands = $(COPY_FILE) \"$$shell_path($$PWD\\logging.conf)\" \"$$shell_path($$DESTDIR)\"
first.depends = $(first) copydata_config copydata_icons copydata_logconfig
export(first.depends)
export(copydata_config.commands)
export(copydata_icons.commands)
export(copydata_logconfig.commands)
QMAKE_EXTRA_TARGETS += first copydata_config copydata_icons copydata_logconfig
