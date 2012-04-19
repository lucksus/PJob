TEMPLATE = app
QT += opengl
CONFIG += debug_and_release qt
CONFIG -= flat
TARGET = PQueue
DESTDIR = ../../bin
DEPENDPATH += . ../PQueueModel ../CondorC++Adapter ../PJobFile
INCLUDEPATH += . ../PQueueModel ../CondorC++Adapter ../PJobFile ../PJobClient
QT += script xml network
LIBS += -L. -L../../lib
unix: LIBS += -lboost_program_options-mt
macx:LIBS += -framework Glut -framework veclib

include(../src.pri)

build_pass:CONFIG(debug, debug|release) {
    mac: LIBS +=  -lPJobFile_debug -lPQueueModel_debug -lCondorC++Adapter_debug -lPJobClient_debug
    !mac: LIBS +=  -lPJobFiled -lPQueueModeld -lCondorC++Adapterd -lPJobClientd
    mac: POST_TARGETDEPS += ../../lib/libPJobFile_debug.a ../../lib/libPQueueModel_debug.a ../../lib/libCondorC++Adapter_debug.a ../../lib/libPJobClient_debug.a
    unix: POST_TARGETDEPS += ../../lib/libPJobFiled.a ../../lib/libPQueueModeld.a ../../lib/libCondorC++Adapterd.a ../../lib/libPJobClientd.a
    win32: POST_TARGETDEPS += ../../lib/PJobFiled.lib ../../lib/PQueueModeld.lib ../../lib/CondorC++Adapterd.lib ../../lib/PJobClientd.lib
} else {
    LIBS +=  -lPJobFile -lPQueueModel -lCondorC++Adapter -lPJobClient
    !win32: POST_TARGETDEPS += ../../lib/libPJobFile.a ../../lib/libPQueueModel.a ../../lib/libCondorC++Adapter.a ../../lib/libPJobClient.a
    win32: POST_TARGETDEPS += ../../lib/PJobFile.lib ../../lib/PQueueModel.lib ../../lib/CondorC++Adapter.lib ../../lib/PJobClient.lib
}

unix:LIBS += -llapack -lblas

win32{
    QMAKE_POST_LINK='copy findRegion.js  $$DESTDIR & copy parametervariation.js $$DESTDIR'
}else{
    QMAKE_POST_LINK='cp findRegion.js  $$DESTDIR & cp parametervariation.js $$DESTDIR'
}

# Input
HEADERS += MainWindow.h \
	SettingsDialog.h \
	NonEquidistantSlider.h \
	PlotWidget.h \
	Drawable.h \
	Box.h \
	ResultVisualization.h \
	ScriptedUserInputDialog.h \
	InterpolationFunctionVisualization.h \
	ResultModel.h
FORMS += MainWindow.ui SettingsDialog.ui
SOURCES += main.cpp \
	MainWindow.cpp \
	SettingsDialog.cpp \
	NonEquidistantSlider.cpp \
	PlotWidget.cpp \
	Box.cpp \
	ResultVisualization.cpp \
	ScriptedUserInputDialog.cpp \
	Drawable.cpp \
	InterpolationFunctionVisualization.cpp \
	ResultModel.cpp
RESOURCES += MainWindow.qrc \
    Resources/images.qrc
RC_FILE = windows.rc
ICON = Resources/MacOSXIcon.icns
