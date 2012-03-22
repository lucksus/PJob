QT += xml
CONFIG += debug_and_release
TEMPLATE = app
TARGET = 
DEPENDPATH += . ../PJobFile
INCLUDEPATH += . ../PJobFile
ICON = Resources/MacOSXIcon.icns
LIBS += ../../lib
DESTDIR=../../bin

include(../src.pri)

build_pass:CONFIG(debug, debug|release) {
    mac: LIBS +=  -lPJobFile_debug
    !mac: LIBS +=  -lPJobFiled
} else {
    LIBS +=  -lPJobFile
}

# Input
HEADERS += PJobDirTree.h \
			PJobDirModel.h \
                        PJobFileEditor.h
SOURCES += main.cpp \
			PJobDirTree.cpp \
			PJobDirModel.cpp \
                        PJobFileEditor.cpp
			
FORMS += PJobFileEditor.ui
RESOURCES += PJobFileEditor.qrc
RC_FILE = windows.rc
