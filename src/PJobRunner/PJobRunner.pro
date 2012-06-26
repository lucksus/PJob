#-------------------------------------------------
#
# Project created by QtCreator 2012-03-27T13:35:32
#
#-------------------------------------------------

QT       += core xml script network
QT       -= gui
CONFIG += debug_and_release
TARGET = PJobRunner
CONFIG   += console
CONFIG   -= app_bundle
DEPENDPATH += . ../PJobFile
INCLUDEPATH += . ../PJobFile
TEMPLATE = app

LIBS += -L../../lib
DESTDIR=../../bin

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

SOURCES += main.cpp \
    scriptengine.cpp \
    tcpserver.cpp \
    session.cpp \
    sessionthread.cpp \
    dataconnectionthread.cpp \
    pjobrunnerservice.cpp \
    ticketdispatcher.cpp \
    dataconnectionserver.cpp

HEADERS += \
    scriptengine.h \
    tcpserver.h \
    session.h \
    sessionthread.h \
    dataconnectionthread.h \
    pjobrunnerservice.h \
    ticketdispatcher.h \
    dataconnectionserver.h

include(qtservice/src/qtservice.pri)
