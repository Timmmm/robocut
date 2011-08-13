# -------------------------------------------------
# Project created by QtCreator 2010-10-03T18:21:30
# -------------------------------------------------
TARGET = robocut
VERSION = 1.0.8
TEMPLATE = app
INSTALLS += icon \
	    target
target.path = /usr/bin
icon.files += ./images/robocut.xpm
icon.path = /usr/share/pixmaps/
QMAKE_EXTRA_TARGETS += release tarball
                  
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
    
release.depends = tarball

tarball.target = $${TARGET}-$${VERSION}.tar.gz
tarball.commands = \
        $(DEL_FILE) -r $${TARGET}-$${VERSION} ; \
        $(MKDIR) $${TARGET}-$${VERSION} ; \
        $(COPY_DIR) * $${TARGET}-$${VERSION}/ ; \
        $(DEL_FILE) $${TARGET}-$${VERSION}/*.pro.user \
                $${TARGET}-$${VERSION}/$${TARGET}-$${VERSION}.tar.gz \
                $(DEL_FILE) -r $${TARGET}-$${VERSION}/$${TARGET}-$${VERSION} \
                $${TARGET}-$${VERSION}/autodist.sh ; \
                $(DEL_FILE) -r $${TARGET}-$${VERSION}/$${TARGET}-$${VERSION} \
                $${TARGET}-$${VERSION}/umbrello_stuff.xmi ; \
                $(DEL_FILE) -r $${TARGET}-$${VERSION}/$${TARGET}-$${VERSION} \
                $${TARGET}-$${VERSION}/robocut ; \
                $(DEL_FILE) -r $${TARGET}-$${VERSION}/$${TARGET}-$${VERSION} \
                $${TARGET}-$${VERSION}/Makefile ; \
        tar -cz --exclude=.svn --exclude=*.tar.gz -f $$tarball.target $${TARGET}-$${VERSION} ; \
        $(DEL_FILE) -r $${TARGET}-$${VERSION}
