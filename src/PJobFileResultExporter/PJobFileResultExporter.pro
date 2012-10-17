#-------------------------------------------------
#
# Project created by QtCreator 2012-10-10T12:06:27
#
#-------------------------------------------------

TEMPLATE = app
TARGET = pjob_export
QT       += core xml
QT       -= gui
CONFIG   += console debug_and_release
DESTDIR = ../../bin

INCLUDEPATH += ../PJobFile
DEPENDPATH += . ../PJobFile

include(../src.pri)
!macx:unix: LIBS += -lboost_program_options
macx: LIBS += -lboost_program_options-mt

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


SOURCES += main.cpp
