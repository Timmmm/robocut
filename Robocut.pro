# -------------------------------------------------
# Project created by QtCreator 2010-10-03T18:21:30
# -------------------------------------------------
TARGET = Robocut
TEMPLATE = app
SOURCES += main.cpp \
    MainWindow.cpp \
    Plotter.cpp \
    Common.cpp \
    PlotterPage.cpp \
    PathPaintEngine.cpp \
    CutDialog.cpp
HEADERS += MainWindow.h \
    Plotter.h \
    NoCopy.h \
    Common.h \
    PlotterPage.h \
    PathPaintEngine.h \
    CutDialog.h
FORMS += MainWindow.ui \
    CutDialog.ui

LIBS += -lusb-1.0

QT += svg

RESOURCES += \
    resources.qrc
