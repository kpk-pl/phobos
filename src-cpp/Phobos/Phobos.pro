QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Phobos
TEMPLATE = app
CONFIG += c++14

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += Phobos.cpp\
    MainWindow.cpp \
    ViewStack.cpp \
    Config.cpp \
    AllSeriesView.cpp \
    ImageWidget.cpp \
    ImageProcessing/Metrics.cpp \
    ImageProcessing/LoaderThread.cpp \
    ImageProcessing/FormatConversion.cpp \
    PhotoContainers/Item.cpp \
    PhotoContainers/Series.cpp \
    PhotoContainers/Set.cpp \
    ImageProcessing/ScalePixmap.cpp \
    ConfigExtension.cpp \
    PhotoItemWidget.cpp \
    PhotoItemWidgetAddon.cpp \
    SeriesViewBase.cpp \
    NumSeriesView.cpp \
    Utils/LayoutClear.cpp \
    ImageProcessing/ColoredPixmap.cpp \
    NavigationBar.cpp \
    RowSeriesView.cpp \
    HorizontalScrollArea.cpp \
    ImageProcessing/Bluriness.cpp \
    ImageProcessing/Noisiness.C \
    ImageProcessing/Histogram.cpp \
    ImageProcessing/MetricsAggregate.cpp

HEADERS  += MainWindow.h \
    ViewStack.h \
    Config.h \
    AllSeriesView.h \
    ConfigExtension.h \
    ImageWidget.h \
    ImageProcessing/Metrics.h \
    ImageProcessing/LoaderThread.h \
    ImageProcessing/ScalePixmap.h \
    ImageProcessing/FormatConversion.h \
    PhotoContainers/Series.h \
    PhotoContainers/Set.h \
    PhotoContainers/Item.h \
    PhotoItemWidget.h \
    PhotoItemWidgetAddon.h \
    SeriesViewBase.h \
    NumSeriesView.h \
    Utils/LayoutClear.h \
    ImageProcessing/ColoredPixmap.h \
    Utils/Algorithm.h \
    Utils/Focused.h \
    NavigationBar.h \
    RowSeriesView.h \
    HorizontalScrollArea.h \
    HeightResizeableInterface.h \
    ViewDescription.h \
    ImageProcessing/Bluriness.h \
    ImageProcessing/Noisiness.h \
    ImageProcessing/Histogram.h \
    ImageProcessing/MetricsAggregate.h

INCLUDEPATH += cpptoml
INCLUDEPATH += boost
INCLUDEPATH += opencv/include

LIBS += $$PWD/opencv/bin/libopencv_core320.dll
LIBS += $$PWD/opencv/bin/libopencv_imgcodecs320.dll
LIBS += $$PWD/opencv/bin/libopencv_imgproc320.dll

CONFIG(release, debug|release): DESTDIR = $$OUT_PWD/release
CONFIG(debug, debug|release): DESTDIR = $$OUT_PWD/debug

copydata_config.commands = $(COPY_FILE) \"$$shell_path($$PWD\\config.toml)\" \"$$shell_path($$DESTDIR)\"
copydata_icons.commands = $(COPY_DIR) \"$$shell_path($$PWD\\icon)\" \"$$shell_path($$DESTDIR\\icon)\"
first.depends = $(first) copydata_config copydata_icons
export(first.depends)
export(copydata_config.commands)
export(copydata_icons.commands)
QMAKE_EXTRA_TARGETS += first copydata_config copydata_icons
