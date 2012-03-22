#include <QtCore/QString>
#include <QtGui/QMainWindow>
#include <QtCore/QDir>
#include <QtGui/QMessageBox>
#include "PJobFileEditor.h"

int main(int argc, char** argv){
	QApplication app(argc,argv);

	QString jobFilePath;
	if(argc == 2) jobFilePath = QString(argv[1]);
	

	PJobFileEditor mainWindow(jobFilePath);
	mainWindow.show();
	return app.exec();
}
