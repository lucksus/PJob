TEMPLATE = lib
CONFIG += qt staticlib 
CONFIG += debug_and_release
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR=../../lib

include(../src.pri)

# Input
HEADERS += Condor.h \
			LogAdapter.h \
    CondorJob.h
SOURCES += Condor.cpp \
			LogAdapter.cpp \
    CondorJob.cpp
			#test.cpp
