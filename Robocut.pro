TARGET = robocut

VERSION = 1.0.11

TEMPLATE = app

QT += widgets svg
CONFIG += c++14
                         
SOURCES += main.cpp \
    MainWindow.cpp \
    Plotter.cpp \
    Common.cpp \
    PathPaintEngine.cpp \
    CutDialog.cpp \
    CuttingDialog.cpp \
    CuttingThread.cpp \
    PathPaintPage.cpp \
    PathSorter.cpp \
    ProgramOptions.cpp
HEADERS += MainWindow.h \
    Plotter.h \
    NoCopy.h \
    Common.h \
    PathPaintEngine.h \
    CutDialog.h \
    CuttingDialog.h \
    CuttingThread.h \
    PathPaintPage.h \
    PathSorter.h \
    ProgramOptions.h
FORMS += MainWindow.ui \
    CutDialog.ui \
    CuttingDialog.ui
RESOURCES += \
    Resources.qrc

DEFINES += ROBOCUT_VERSION=\\\"$$VERSION\\\"

unix:LIBS += -L/usr/local/lib -lusb-1.0
unix:INCLUDEPATH += /usr/local/include

win32:LIBS += $$_PRO_FILE_PWD_/libusb-windows/MS64/static/libusb-1.0.lib
win32:INCLUDEPATH += $$_PRO_FILE_PWD_/libusb-windows

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.12

OTHER_FILES += \
    libusb/CMakeLists.txt \
    CMakeLists.txt \
    WindowsResources.rc \
    Readme.md \
    COPYING \
    .travis.yml \
    .appveyor.yml


