TEMPLATE = app
TARGET = 
DEPENDPATH += . ../PJobFile 
INCLUDEPATH += . ../PJobFile
CONFIG += console
CONFIG += debug_and_release
LIBS += -L../../lib -lboost_program_options
DESTDIR = ../../bin

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

# Input
HEADERS += PJobOutput.h
SOURCES += PJobCmdLine.cpp PJobOutput.cpp
