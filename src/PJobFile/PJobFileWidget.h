#pragma once
#include <QtGui/QWidget>
#include <QtGui/QStandardItemModel>
#include <QItemDelegate>
#include "ui_PJobWidget.h"
#include "../PJobFileEditor/PJobDirModel.h"

class PJobFile;
class PJobFileWidget :	public QWidget
{
Q_OBJECT
public:
	PJobFileWidget(PJobFile* jobFile, QWidget* parent = 0);
	void refresh();

signals:
	void changed();

private slots:
	void mainScriptChanged();

	// Rechtsklick Menü für das Resources Tree View.
	void showResourcesContextMenu(const QPoint& pos);
	void onResourcesActionExtract();
	void onResourcesActionRename();
	void onResourcesActionDelete();
	void onResourcesActionMakeDir();
	void onResourcesActionAddResources();

	void resourcesChanged();
	void parametersChanged();
	void resultsChanged();

	void on_parametersAddButton_clicked();
	void on_parametersRemoveButton_clicked();

	void on_addResultFileButton_clicked();
	void on_removeResultFileButton_clicked();
	void on_addResultButton_clicked();
	void on_removeResultButton_clicked();

        void on_listOfApps_currentItemChanged(QListWidgetItem * current, QListWidgetItem * previous);
        void on_listOfApps_itemChanged(QListWidgetItem*);
        void on_addApp_clicked();
        void on_removeApp_clicked();
        void on_defaultApp_currentIndexChanged(QString);
        void on_applicationName_textChanged(const QString& text);
        void on_applicationVersion_textChanged(const QString& text);
        void on_applicationExecutable_textChanged(const QString& text);
        void on_applicationParametersPattern_textChanged(const QString& text);
        void on_applicationPlatform_currentIndexChanged(int);


private:
	Ui::PJobWidget ui;
	PJobFile* m_pjobFile;

	PJobDirModel* m_pjobDirModel;
	PJobDirModel* m_runDirModel;
        PJobDirModel* m_applicationFilesModel;

	QList<QAction*> m_resourcesGeneralActions;
	QList<QAction*> m_resourcesFileActions;
	QModelIndex m_clickedIndex;

	QStandardItemModel m_parametersModel;
	QStandardItemModel m_resultsModel;
        QItemDelegate* m_resultsDelegate;
	void syncParametersModelToFile();
	void syncParametersFileToModel();
	void syncResultsModelToFile();
	void syncResultsFileToModel();
        void syncApplicationsFileToModel();
        void syncCurrentApplicationModelToFile();

        bool m_applicationPageIsConstructing;
};



class ResultsTypeDelegate : public QItemDelegate
 {
     Q_OBJECT

 public:
     ResultsTypeDelegate(QObject *parent = 0);

     QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;

     void setEditorData(QWidget *editor, const QModelIndex &index) const;
     void setModelData(QWidget *editor, QAbstractItemModel *model,
                       const QModelIndex &index) const;
 };

