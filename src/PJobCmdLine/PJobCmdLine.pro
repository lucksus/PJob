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
} else {
    LIBS +=  -lPJobFile
}

# Input
HEADERS += PJobOutput.h
SOURCES += PJobCmdLine.cpp PJobOutput.cpp
