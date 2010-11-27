# -------------------------------------------------
# Project created by QtCreator 2010-10-03T18:21:30
# -------------------------------------------------
TARGET = Robocut
VERSION = 1.0.3
TEMPLATE = app
INSTALLS += icon \
	    target
target.path = /usr/bin
icon.files += ./images/robocut.xpm
icon.path = /usr/share/pixmaps/
                         
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

LIBS += -lusb-1.0

QT += svg

RESOURCES += \
    resources.qrc

QMAKE_COPY += \
    readme.txt \
    install.txt \
    changelog.txt \
    images/cap-blue.png \
    images/cap-pen.png \
    images/cap-red.png \
    images/cap-yellow.png \
    images/icon.png \
    images/robocut.xpm \
    examples/letter_reg-marks.svg