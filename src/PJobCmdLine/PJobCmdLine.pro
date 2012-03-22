TEMPLATE = app
TARGET = 
DEPENDPATH += . ../PJobFile 
INCLUDEPATH += . ../PJobFile
CONFIG += console
LIBS += -L../../lib -lboost_program_options

include(../src.pri)

build_pass:CONFIG(debug, debug|release) {
    mac: LIBS +=  -lPJobFile_debug
    !mac: LIBS +=  -lPJobFile
} else {
    LIBS +=  -lPJobFile
}

# Input
HEADERS += PJobOutput.h
SOURCES += PJobCmdLine.cpp PJobOutput.cpp
