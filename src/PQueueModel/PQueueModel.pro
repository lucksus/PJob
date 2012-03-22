TEMPLATE = lib
CONFIG += qt staticlib 
CONFIG += debug_and_release
DEPENDPATH += . ../CondorC++Adapter ../../PJobFile/PJobFile
INCLUDEPATH += . ../CondorC++Adapter ../../PJobFile/PJobFile ../../3rdparty/boost
LIBS += -L../../lib 
QT += script

include(../src.pri)

# Input
HEADERS +=  PhotossJob.h \
	PhotossJobSubmitStrategy.h \
	PhotossJobSubmitCondor.h \
	PhotossJobSubmitLocal.h \
	PhotossJobSubmitStrategyProcessHolder.h \
	PJobFileRepository.h \
	PQueueController.h \
	Results.h \
	Settings.h \
	LatinHypercubeDesign.h \
	Logger.h \
	InterpolationFunction.h \
	Scripter.h \
	ScriptFunctions.h \
	SimulatedAnnealing.h \
	Tokenizer.h \
	EngineThread.h \
	UserInputMediator.h \
	ResultLoaderAndSaver.h \
	RandomGenerator.h
SOURCES += PhotossJob.cpp \
	PhotossJobSubmitStrategy.cpp \
	PhotossJobSubmitCondor.cpp \
	PhotossJobSubmitLocal.cpp \
	PhotossJobSubmitStrategyProcessHolder.cpp \
	PJobFileRepository.cpp \
	PQueueController.cpp \
	Results.cpp \
	Settings.cpp \
	LatinHypercubeDesign.cpp \
	Logger.cpp \
	InterpolationFunction.cpp \
	Scripter.cpp \
	ScriptFunctions.cpp \
	SimulatedAnnealing.cpp \
	Tokenizer.cpp \
	UserInputMediator.cpp \
	ResultLoaderAndSaver.cpp \
	RandomGenerator.cpp
