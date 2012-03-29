QT += xml
TEMPLATE = app
CONFIG += debug_and_release
CONFIG += exceptions
DEPENDPATH += . ../PJobFile
INCLUDEPATH += . ../PJobFile
ICON = Resources/MacOSXIcon.icns
LIBS += -L../../lib
DESTDIR=../../bin

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
