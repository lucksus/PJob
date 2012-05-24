#pragma once
#include <QtCore/QList>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QSortFilterProxyModel>
#include <QtCore/QSignalMapper>
#include "PhotossJob.h"
#include "ui_MainWindow.h"
#include "UserInputMediator.h"
#include "ResultModel.h"
#include "MainWindowWithProgressPopups.h"
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QHostInfo>

class NonEquidistantSlider;
class PlotWidget;
class MainWindow : public MainWindowWithProgressPopups, public UserInputHandler{
Q_OBJECT
public:
	MainWindow();

	virtual QScriptValue userInputForScript(QScriptContext *context, QScriptEngine *engine);
	void viewResult(QString pjob_file, QString result);

private:
	Ui::MainWindow ui;
	QHash<QListWidgetItem*,Job*> m_jobs;

	QHash<QString,QTreeWidgetItem*> m_phoFileItemsNavigator;
	QHash<QTreeWidgetItem*, QHash<QString,QTreeWidgetItem*> > m_resultItemsNavigator;

	QListWidgetItem* itemForJob(Job*);

	void visualizerSelectResult(QString phoFile, QString result);
	void fillParametersBox(QString);

	QList<QFrame*> m_parameterFrames;
	QList<NonEquidistantSlider*> m_parameterSliders;
	QButtonGroup m_x_axis_button_group;
	QButtonGroup m_y_axis_button_group;

	PlotWidget* m_plotWidget;

	QSignalMapper m_builtInScriptsActionsMapper;
	QString m_runningScript;

        QHash<QHostAddress, QListWidgetItem*> m_pjob_runner_items;

private slots:
	void on_browsePJobFileButton_clicked();
	void on_addParameterButton_clicked();
	void on_delParameterButton_clicked();
	void on_pjobFile_textChanged(QString);
	void on_addJobButton_clicked();
	void on_jobUpButton_clicked();
	void on_jobDownButton_clicked();
	void on_jobDeleteButton_clicked();
	void on_startButton_clicked();
	void on_stopButton_clicked();
	void on_actionEdit_triggered();
	void on_actionExport_To_CSV_triggered();
	void on_actionImport_From_CSV_triggered();
	void on_actionImport_From_PJob_triggered();
	void on_navigatorResultsTreeWidget_itemClicked(QTreeWidgetItem*);
	void on_scriptLineEdit_returnPressed();
	void on_actionLoad_script_from_file_triggered();
	void on_runScriptButton_clicked();
	void on_stopScriptButton_clicked();

	void jobCreated(Job* j, unsigned int position);
	void jobRemoved(Job* j);
	void jobMoved(Job* j, unsigned int position);
	void jobStateChanged(Job* j, Job::State state);
	void started();
	void stopped();
	void newValue();
	/*! Baut den Baum zur Auswahl der Ergebnisse für die Visualisierung*/
	void newValue(QString phoFile, QString result, QHash<QString,double> parameters, double value);
	void updatePJobFileSelector(QString pjobFile); //!< Update der ComboBox zur Auswahl des PJobs für das ResultModel
	void initialSortResultView(); //!< Initialisieren der Sortierung für das ResultView
	
	void updateButtons();
	void updateParametersBox();
	void showVisualization();
	void showVisualization(QString pjob_file, QString result);

	void addLoadedScriptToListWidget(QString);
	void scriptStarted(QString);
	void scriptFinished(QString,QString);
	void setScriptProgress(QString engineName, unsigned int progress);

	void openScriptedUserInputDialog(QScriptContext *context, QScriptEngine *engine);

	void builtInScriptTriggered(const QString&);

	void on_actionMruAction1_triggered();
	void on_actionMruAction2_triggered();
	void on_actionMruAction3_triggered();

	void on_loadPreviousRunsButton_clicked();
        void hide_splash_screen();

        void found_new_pjob_runner(QHostAddress);
        void lost_pjob_runner(QHostAddress);
        void probing_host(QHostAddress);
        void lookedUp(const QHostInfo& host);
        void pjob_runner_search_finished();

public:
signals:
	void needScriptedUserInputDialog(QScriptContext *context, QScriptEngine *engine);
private:
	ResultModel* m_resultModel; //!< Model zum Darstellen der Results
	QSortFilterProxyModel* m_proxyResultModel; //!< Proxy Model zum Sortieren des Result Models

	QScriptValue scriptedUserInputDialogResult;
	QMutex scriptedUserInputDialogMutex;
	QWaitCondition scriptedUserInputDialogWaitCondition;

	QStringList mostRecentlyUsedPJOBFiles();
	void mostRecentlyUsedPJOBFilesAdd(QString pjobFilePath);
	void mruToFileMenu();
};
