TEMPLATE = app
TARGET = 
DEPENDPATH += . ../PJobFile 
INCLUDEPATH += . ../PJobFile
CONFIG += console
CONFIG += debug_and_release
LIBS += -L../../lib #-lboost_program_options
DESTDIR = ../../bin

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

# Input
HEADERS += PJobOutput.h
SOURCES += PJobCmdLine.cpp PJobOutput.cpp
