# -------------------------------------------------
# Project created by QtCreator 2010-10-03T18:21:30
# -------------------------------------------------
TARGET = Robocut
TEMPLATE = app
SOURCES += main.cpp \
    MainWindow.cpp \
    Plotter.cpp \
    Common.cpp \
    PathPaintEngine.cpp \
    CutDialog.cpp \
    CuttingDialog.cpp \
    CuttingThread.cpp \
    PathPaintPage.cpp \
    PathSorter.cpp
HEADERS += MainWindow.h \
    Plotter.h \
    NoCopy.h \
    Common.h \
    PathPaintEngine.h \
    CutDialog.h \
    CuttingDialog.h \
    CuttingThread.h \
    PathPaintPage.h \
    PathSorter.h
FORMS += MainWindow.ui \
    CutDialog.ui \
    CuttingDialog.ui

LIBS += -lusb-1.0

QT += svg

RESOURCES += \
    resources.qrc

OTHER_FILES += \
    readme.txt \
    install.txt \
    changelog.txt
