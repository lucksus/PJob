#-------------------------------------------------
#
# Project created by QtCreator 2012-10-17T11:16:02
#
#-------------------------------------------------

QT       += core gui opengl xml
CONFIG += debug_and_release
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
DESTDIR = ../../bin
TARGET = Schlangensicht
TEMPLATE = app
macx:LIBS += -framework Glut
LIBS += -lGLU -lGL

include(../src.pri)

build_pass:CONFIG(debug, debug|release) {
    mac: LIBS +=  -lPJobFile_debug
    !mac: LIBS +=  -lPJobFiled
    mac: POST_TARGETDEPS += ../../lib/libPJobFile_debug.a
    !mac: unix: POST_TARGETDEPS += ../../lib/libPJobFiled.a
    win32: POST_TARGETDEPS += ../../lib/PJobFiled.lib
} else {
    LIBS +=  -lPJobFile
    !win32: POST_TARGETDEPS += ../../lib/libPJobFile.a
    win32: POST_TARGETDEPS += ../../lib/PJobFile.lib
}


INCLUDEPATH += ../PJobFile
DEPENDPATH += ../PJobFile

SOURCES += main.cpp\
        mainwindow.cpp \
    NonEquidistantSlider.cpp \
    ResultVisualization.cpp \
    Drawable.cpp \
    PlotWidget.cpp \
    Box.cpp \
    Results.cpp \
    ResultLoaderAndSaver.cpp \
    ResultModel.cpp

HEADERS  += mainwindow.h \
    ResultVisualization.h \
    PlotWidget.h \
    NonEquidistantSlider.h \
    Box.h \
    Drawable.h \
    Results.h \
    ResultLoaderAndSaver.h \
    ResultModel.h

FORMS    += mainwindow.ui
