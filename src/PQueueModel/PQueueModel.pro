TEMPLATE = lib
CONFIG += qt staticlib 
CONFIG += debug_and_release
DEPENDPATH += . ../CondorC++Adapter ../PJobFile
INCLUDEPATH += . ../CondorC++Adapter ../PJobFile ../PJobClient
LIBS += -L../../lib 
QT += script
DESTDIR=../../lib
include(../src.pri)

# Input
HEADERS +=  \
	PJobFileRepository.h \
	Results.h \
	Settings.h \
	LatinHypercubeDesign.h \
	Logger.h \
	InterpolationFunction.h \
	Scripter.h \
	ScriptFunctions.h \
	SimulatedAnnealing.h \
	EngineThread.h \
	UserInputMediator.h \
	ResultLoaderAndSaver.h \
	RandomGenerator.h \
    pjobrunnerpool.h \
    Job.h \
    PJobRunnerSessionThread.h \
    Workspace.h
SOURCES += \
	PJobFileRepository.cpp \
	Results.cpp \
	Settings.cpp \
	LatinHypercubeDesign.cpp \
	Logger.cpp \
	InterpolationFunction.cpp \
	Scripter.cpp \
	ScriptFunctions.cpp \
	SimulatedAnnealing.cpp \
	UserInputMediator.cpp \
	ResultLoaderAndSaver.cpp \
	RandomGenerator.cpp \
    pjobrunnerpool.cpp \
    Job.cpp \
    PJobRunnerSessionThread.cpp \
    Workspace.cpp
