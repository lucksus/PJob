#include "PJobFileEditor.h"
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtCore/QSettings>

PJobFileEditor::PJobFileEditor(QString pjobFile)
: m_pjobFile(0), m_pjobFileWidget(0), m_unsavedChanges(false)
{
	ui.setupUi(this);

	if(pjobFile.length()){
		open(pjobFile);
		ui.actionNew->setEnabled(false);
		ui.actionOpen->setEnabled(false);
		ui.actionDelete_runs->setEnabled(true);
	}else{
		ui.actionSave->setEnabled(false);
		ui.actionClose->setEnabled(false);
		ui.actionDelete_runs->setEnabled(false);
	}

	connect(ui.menuRecentlyUsed, SIGNAL(aboutToShow()), this, SLOT(populateRecentlyUsedMenu()));
}

void PJobFileEditor::open(QString pjobFile){
	m_pjobFile = new PJobFile(pjobFile);	
	m_pjobFile->setSaveAutomatically(false);
	m_pjobFileWidget = new PJobFileWidget(m_pjobFile, this);
        connect(m_pjobFile,SIGNAL(changed()),this,SLOT(pjobTempChanged()));
	connect(m_pjobFileWidget,SIGNAL(changed()),this,SLOT(pjobTempChanged()));
	setCentralWidget(m_pjobFileWidget);
	ui.actionNew->setEnabled(false);
	ui.actionOpen->setEnabled(false);
	ui.actionSave->setEnabled(true);
	ui.actionClose->setEnabled(true);
	ui.actionDelete_runs->setEnabled(true);
        ui.actionSave_without_runs->setEnabled(true);
	QFileInfo fileInfo(pjobFile);
        setWindowTitle(fileInfo.fileName()+"[*] - PJOB Editor");
	addFileToRecentlyUsed(pjobFile);
	add_calculator_object(m_pjobFile);
}

void PJobFileEditor::close(){
	if(m_unsavedChanges){
		int ret = QMessageBox::question(this, tr("PJOB Editor"),
			tr("The document has been modified.\n"
			"Do you want to save your changes?"),
			QMessageBox::Save | QMessageBox::Discard
			| QMessageBox::Cancel,
			QMessageBox::Save);

		switch(ret){
			case QMessageBox::Save:
				m_pjobFile->save();
				break;
			case QMessageBox::Discard:
				break;
			case QMessageBox::Cancel:
			default:
				return;
		}
	}
	if(m_pjobFileWidget){ delete m_pjobFileWidget; m_pjobFileWidget=0;}
	if(m_pjobFile) {delete m_pjobFile; m_pjobFile=0;}
	ui.actionNew->setEnabled(true);
	ui.actionOpen->setEnabled(true);
	ui.actionSave->setEnabled(false);
	ui.actionClose->setEnabled(false);
	ui.actionDelete_runs->setEnabled(false);
        ui.actionSave_without_runs->setEnabled(false);
	setWindowTitle("PJOB Editor");
	m_unsavedChanges = false;
	remove_calculator_object(m_pjobFile);
}


PJobFileEditor::~PJobFileEditor(void)
{
	close();
}

void PJobFileEditor::on_actionNew_triggered(){
	QString fileName = QFileDialog::getSaveFileName(this,
		tr("Select destination for new PJOB file"), "", tr("PJOB Files (*.pjob)"));
	if(fileName=="") return;
        QFile file(fileName);
        if(file.exists() && !file.remove()){
            QMessageBox::critical(this, "PJOB Editor", "Could not delete file "+fileName+"!",QMessageBox::Ok);
            return;
        }
	open(fileName);
}

void PJobFileEditor::on_actionOpen_triggered(){
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Select existing PJOB file"), "", tr("PJOB Files (*.pjob)"));
	if(fileName=="") return;
	open(fileName);
}

void PJobFileEditor::on_actionSave_triggered(){
	m_pjobFile->save();
	m_unsavedChanges = false;
	setWindowModified(false);
}

void PJobFileEditor::on_actionClose_triggered(){
	close();
}

void PJobFileEditor::pjobTempChanged(){
	m_unsavedChanges = true;
	setWindowModified(true);
}

QList<QString> PJobFileEditor::recentlyUsedFiles() const{
	QSettings settings("lucksus", "PJobEditor");
	settings.beginGroup("application/recently_used");
	QList<QString> recentFiles;
	QString key;
	foreach(key, settings.allKeys())
		recentFiles.insert(key.toInt(), settings.value(key).toString());
	return recentFiles;
}

void PJobFileEditor::setRecentlyUsedFiles(QList<QString> files){
	QSettings settings("lucksus", "PJobEditor");
	settings.beginGroup("application/recently_used");
	QString key;
	foreach(key, settings.allKeys())
		settings.remove(key);
	for(int x = 0; x < files.size(); x++)
		settings.setValue(QString("%1").arg(x), files[x]);
}

void PJobFileEditor::openRecentlyUsedFile(){
	QAction* recentFile = qobject_cast<QAction*>(sender());
	if(recentFile) open(recentFile->text());
}

void PJobFileEditor::populateRecentlyUsedMenu(){
	ui.menuRecentlyUsed->clear();
	QString filePath;
	QAction* action;
	foreach(filePath, recentlyUsedFiles()){
		if(!filePath.isEmpty()){
			action = ui.menuRecentlyUsed->addAction(filePath);
			connect(action, SIGNAL(triggered()), this, SLOT(openRecentlyUsedFile()));
		}
	}
	if(!ui.menuRecentlyUsed->isEmpty()) ui.menuRecentlyUsed->addSeparator();
	action = ui.menuRecentlyUsed->addAction("Clear");
	connect(action, SIGNAL(triggered()), this, SLOT(clearRecentlyUsedFiles()));
}

void PJobFileEditor::addFileToRecentlyUsed(QString filePath){
	QList<QString> recentFiles = recentlyUsedFiles();
	if(!recentFiles.empty())
		if(recentFiles[0] == filePath) return;
	recentFiles.removeAll(filePath);
	recentFiles.prepend(filePath);
	setRecentlyUsedFiles(recentFiles);
}

void PJobFileEditor::clearRecentlyUsedFiles(){
	setRecentlyUsedFiles(QList<QString>());
	populateRecentlyUsedMenu();
}

void PJobFileEditor::closeEvent(QCloseEvent *event)
{
	if(m_unsavedChanges){
		int ret = QMessageBox::question(this, tr("PJOB Editor"),
			tr("The document has been modified.\n"
			"Do you want to save your changes?"),
			QMessageBox::Save | QMessageBox::Discard
			| QMessageBox::Cancel,
			QMessageBox::Save);
		switch(ret){
			case QMessageBox::Save:
				m_pjobFile->save();
				event->accept();
				break;
			case QMessageBox::Discard:
				event->accept();
				break;
			case QMessageBox::Cancel:
			default:
				event->ignore();
				break;
		}
		m_unsavedChanges = false;
	}
}


void PJobFileEditor::on_actionDelete_runs_triggered(){
	if(QMessageBox::Yes == QMessageBox::question(this, "PJobEditor: delete runs?", "Really delete all run directories?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
		m_pjobFile->remove_all_runs();
	m_pjobFileWidget->refresh();
}

void PJobFileEditor::on_actionSave_without_runs_triggered(){
    QString fileName = QFileDialog::getSaveFileName(this,
            tr("Select destination for PJOB file without runs"), "", tr("PJOB Files (*.pjob)"));
    if(fileName=="") return;
    QFile file(fileName);
    if(file.exists() && !file.remove()){
        QMessageBox::critical(this, "PJOB Editor", "Could not delete file "+fileName+"!",QMessageBox::Ok);
        return;
    }
    file.open(QIODevice::WriteOnly);
    QByteArray *arr = m_pjobFile->raw_without_results();
    file.write(*arr);
    delete arr;
    file.close();
}
