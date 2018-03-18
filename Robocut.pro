TARGET = robocut

VERSION = 1.0.11

TEMPLATE = app

QT += widgets svg
CONFIG += c++11
                         
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
    resources.qrc

DEFINES += ROBOCUT_VERSION=\\\"$$VERSION\\\"

unix:LIBS += -L/usr/local/lib -lusb-1.0
unix:INCLUDEPATH += /usr/local/include

win32:LIBS += $$_PRO_FILE_PWD_/libusb-windows/MS64/static/libusb-1.0.lib
win32:INCLUDEPATH += $$_PRO_FILE_PWD_/libusb-windows

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.12

QMAKE_COPY += \
    readme.txt \
    install.txt \
    changelog \
    images/cap-blue.png \
    images/cap-pen.png \
    images/cap-red.png \
    images/cap-yellow.png \
    images/icon.png \
    images/robocut.xpm \
    images/robocut.xcf \
    images/cap.xcf \
    images/pen.xcf \
    examples/letter_reg-marks.svg

DISTFILES += \
    robocut.spec \
    Readme.md

INSTALLS += icon \
    target

target.path = /usr/bin
icon.files += ./images/robocut.xpm
icon.path = /usr/share/pixmaps/

# Instructions for Windows release:

# In Qt Creator, go to Projects->Run
# Add->Deployment configuration
# Rename->"Create Windows Release"
# Add Deploy Step->Custom Process Step
#
# Command:      C:\Qt\5.4\mingw491_32\bin\windeployqt.exe
# Argument:     --dir Robocut --compiler-runtime robocut.exe
# Working dir:  %{buildDir}/%{CurrentBuild:Type}
#
# Add another process step:
#
# Command:      C:\Windows\System32\xcopy.exe
# Argument:     /Y robocut.exe Robocut
# Working dir:  %{buildDir}/%{CurrentBuild:Type}
#
# Select that deployment method and run the program. You should be given a directory with all the required files in
# it. Before you zip and upload it, exit Qt creator and rename c:\Qt to c:\Qt2. Re-run it to verify that it works
# without the Qt SDK installed.
#
# Note that there is a minor flaw in this deployment method - if you later update your Qt SDK it will find the updated
# DLLs in c:\Qt *before* it finds the local Qt DLLs and won't run properly. The solution is to add a qt.conf
# file, but that makes development more of a faff. Anyway, Qt Windows deployment is a big mess.
