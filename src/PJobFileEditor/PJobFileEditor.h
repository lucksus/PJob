#pragma once
#include <QtGui/QCloseEvent>
#include "ui_PJobFileEditor.h"
#include "PJobFileWidget.h"
#include "PJobFile.h"
#include "MainWindowWithProgressPopups.h"

class PJobFileEditor : public MainWindowWithProgressPopups
{
Q_OBJECT
public:
	PJobFileEditor(QString pjobFile="");
	~PJobFileEditor(void);

protected:
	void closeEvent(QCloseEvent *event);
private slots:
	void on_actionNew_triggered();
	void on_actionOpen_triggered();
	void on_actionSave_triggered();
	void on_actionClose_triggered();
	void on_actionDelete_runs_triggered();

	void pjobTempChanged();

	void openRecentlyUsedFile();
	void populateRecentlyUsedMenu();
	void clearRecentlyUsedFiles();

private:
	Ui::PJobFileEditor ui;
	PJobFile* m_pjobFile;
	PJobFileWidget* m_pjobFileWidget;
	bool m_unsavedChanges;

	void open(QString pjobFile);
	void close();
	QList<QString> recentlyUsedFiles() const;
	void addFileToRecentlyUsed(QString filePath);
	void setRecentlyUsedFiles(QList<QString> files);
};
