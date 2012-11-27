CONFIG += testlib 
QT += testlib
TEMPLATE = app
TARGET = 
DEPENDPATH += . 
INCLUDEPATH += . 
LIBS += -L.. 

# Input
HEADERS += ResultsTest.h Results.h
SOURCES += ResultsTest.cpp Results.cpp


