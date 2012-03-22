PRECOMPILED_HEADER =
TEMPLATE = lib
CONFIG += qt staticlib 
CONFIG += debug_and_release
TARGET = PJobFile
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR=../../lib

include(../src.pri)

# Input
HEADERS += FileHelperFunctions.h \
			hash.h \
			PJobFile.h \
			PhotossJobFile.h \
			PJobFileFormat.h \
			PJobFileParameter.h \
			PJobFileParameterDefinition.h \
			PJobFileWidget.h \
			PJobFileError.h \
			PJobFileXMLFunctions.h \
			PJobResult.h \
			PJobResultFile.h \
			../PJobFileEditor/PJobDirModel.h \
            ../PJobFileEditor/PJobDirTree.h \
            Syntaxhighlighter.h \
			MainWindowWithProgressPopups.h
			
SOURCES += FileHelperFunctions.cpp \
			PJobFile.cpp \
			PhotossJobFile.cpp \
			PJobFileFormat.cpp \
			PJobFileParameter.cpp \
			PJobFileParameterDefinition.cpp \
			PJobFileWidget.cpp \
			PJobFileXMLFunctions.cpp \
			PJobResult.cpp \
			PJobResultFile.cpp \
			../PJobFileEditor/PJobDirModel.cpp \
            ../PJobFileEditor/PJobDirTree.cpp \
            Syntaxhighlighter.cpp \
			MainWindowWithProgressPopups.cpp
			
FORMS += PJobWidget.ui
