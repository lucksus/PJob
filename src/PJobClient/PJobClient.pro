#-------------------------------------------------
#
# Project created by QtCreator 2012-04-11T16:16:06
#
#-------------------------------------------------

QT       += core network xml

QT       -= gui

TARGET = PJobClient
CONFIG   += console
CONFIG   += debug_and_release
CONFIG   -= app_bundle

TEMPLATE = app
LIBS += -L../../lib
DEPENDPATH += . ../PJobFile
INCLUDEPATH += . ../PJobFile
DESTDIR=../../bin

include(../src.pri)

build_pass:CONFIG(debug, debug|release) {
    mac: LIBS +=  -lPJobFile_debug
    !mac: LIBS +=  -lPJobFiled
    mac: POST_TARGETDEPS += ../../lib/libPJobFile_debug.a
    !mac: POST_TARGETDEPS += ../../lib/libPJobFiled.a
} else {
    LIBS +=  -lPJobFile
    POST_TARGETDEPS += ../../lib/libPJobFile.a
}
SOURCES += main.cpp
