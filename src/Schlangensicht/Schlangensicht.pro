#-------------------------------------------------
#
# Project created by QtCreator 2012-10-17T11:16:02
#
#-------------------------------------------------

QT       += core gui opengl
CONFIG += debug_and_release
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Schlangensicht
TEMPLATE = app
macx:LIBS += -framework Glut
LIBS += -lGLU -lGL

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
