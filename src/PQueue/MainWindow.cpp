#include "MainWindow.h"
#include <QtGui/QFileDialog>
#include <QtGui/QDockWidget>
#include "Workspace.h"
#include "Logger.h"
#include "SettingsDialog.h"
#include "NonEquidistantSlider.h"
#include <QtGui/QGridLayout>
#include "PlotWidget.h"
#include <assert.h>
#include "Scripter.h"
#include "ScriptedUserInputDialog.h"
#include <QtCore/QTimer>
#include <QtCore/QSettings>
#include <QSplashScreen>
#include <QtNetwork/QHostInfo>
#include "pjobrunnerpool.h"
#include "JobOutputWidget.h"
#include <QRadioButton>

MainWindow::MainWindow(void)
    : m_pjob_file(0)
{
	ui.setupUi(this);

        QTimer::singleShot(1500, this, SLOT(hide_splash_screen()));

	UserInputMediator::getInstance().setUserInputHandler(this);

	setCentralWidget(ui.tabWidget);
	addDockWidget(Qt::BottomDockWidgetArea, ui.logDockWidget);
	ui.logDockWidget->hide();
	addDockWidget(Qt::BottomDockWidgetArea, ui.scriptingConsoleDockWidget);
	ui.scriptingConsoleDockWidget->hide();
	addDockWidget(Qt::BottomDockWidgetArea, ui.scriptFilesDockWidget);
	ui.scriptFilesDockWidget->hide();
	m_plotWidget = new PlotWidget;
	//ui.visualizationBox->setLayout(new QGridLayout);
	QVBoxLayout* layout = dynamic_cast<QVBoxLayout*>(ui.visualizationBox->layout());
	layout->insertWidget(0,m_plotWidget);
	connect(ui.showInterpolant, SIGNAL(stateChanged(int)), m_plotWidget, SLOT(showInterpolant(int)));
	QList<int> sizes1;
	sizes1 << 100 << 500;
	ui.visualizationVerticalSplitter->setSizes(sizes1);
	QList<int> sizes2;
	sizes2 << 600 << 200;
	ui.visualizationHorizontalSplitter->setSizes(sizes2);
	QStringList l1;
	l1 << "Parametername" << "Parametervalue";
	ui.parametersWidget->setColumnCount(2);
	ui.parametersWidget->setHorizontalHeaderLabels(l1);
	ui.parametersWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.parametersWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	// Initialisieren Result View
	ui.pjobFileSelector->setSizeAdjustPolicy(QComboBox::AdjustToContents);
        m_resultModel = new ResultModel(&Workspace::getInstace().getResults(), this);
	QSortFilterProxyModel* m_proxyResultModel = new QSortFilterProxyModel(this);
	m_proxyResultModel->setSourceModel(m_resultModel);
	ui.resultView->setModel(m_proxyResultModel);
	ui.resultView->setSortingEnabled(true);
	connect(ui.pjobFileSelector, SIGNAL(currentIndexChanged(QString)), m_resultModel, SLOT(initialize(QString)));
        connect(&Workspace::getInstace().getResults(), SIGNAL(newValuesSet(QString)), this, SLOT(updatePJobFileSelector(QString)));
        connect(&Workspace::getInstace().getResults(), SIGNAL(newValuesSet(QString)), m_resultModel, SLOT(update(QString)));
	connect(m_resultModel, SIGNAL(layoutChanged()), ui.resultView, SLOT(resizeColumnsToContents()));
	connect(m_resultModel, SIGNAL(modelInitialized()), this, SLOT(initialSortResultView()));

	ui.scriptProgressBar->setHidden(true);

        connect(&Workspace::getInstace(), SIGNAL(jobAdded(Job*,unsigned int)), this, SLOT(jobCreated(Job*,unsigned int)));
        connect(&Workspace::getInstace(), SIGNAL(jobAdded(Job*,unsigned int)), this, SLOT(updateButtons()));
        connect(&Workspace::getInstace(), SIGNAL(jobRemoved(Job*)), this, SLOT(jobRemoved(Job*)));
        connect(&Workspace::getInstace(), SIGNAL(jobRemoved(Job*)), this, SLOT(updateButtons()));
        connect(&Workspace::getInstace(), SIGNAL(jobMoved(Job*, unsigned int)), this, SLOT(jobMoved(Job*, unsigned int)));
	connect(ui.jobsWidget, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtons()));
	updateButtons();

	connect(&Logger::getInstance(),SIGNAL(text(QString)),ui.logTextEdit,SLOT(append(QString)));
        connect(&Workspace::getInstace().getResults(), SIGNAL(newValueSet(QString,QString,QHash<QString,double>,double)), this, SLOT(newValue(QString,QString,QHash<QString,double>,double)));
        connect(&Workspace::getInstace().getResults(), SIGNAL(newValuesSet(QString)), this, SLOT(newValue()));
        connect(&Workspace::getInstace(), SIGNAL(started()), this, SLOT(started()));
        connect(&Workspace::getInstace(), SIGNAL(stopped()), this, SLOT(stopped()));

	connect(&m_x_axis_button_group, SIGNAL(buttonClicked(int)), this, SLOT(updateParametersBox()));
	connect(&m_y_axis_button_group, SIGNAL(buttonClicked(int)), this, SLOT(updateParametersBox()));
	connect(&m_x_axis_button_group, SIGNAL(buttonClicked(int)), this, SLOT(showVisualization()));
	connect(&m_y_axis_button_group, SIGNAL(buttonClicked(int)), this, SLOT(showVisualization()));

	connect(&Scripter::getInstance(), SIGNAL(scriptFileLoaded(QString)), this, SLOT(addLoadedScriptToListWidget(QString)));
	connect(&Scripter::getInstance(), SIGNAL(engineStarted(QString)), this, SLOT(scriptStarted(QString)));
	connect(&Scripter::getInstance(), SIGNAL(engineFinished(QString,QString)), this, SLOT(scriptFinished(QString,QString)));
	connect(&Scripter::getInstance(), SIGNAL(scriptOutput(QString)), ui.scriptOutput, SLOT(insertPlainText(QString)));
	connect(&Scripter::getInstance(), SIGNAL(scriptProgessUpdated(QString, unsigned int)), this, SLOT(setScriptProgress(QString, unsigned int)));

	connect(this,SIGNAL(needScriptedUserInputDialog(QScriptContext*, QScriptEngine*)),this,SLOT(openScriptedUserInputDialog(QScriptContext*, QScriptEngine*)));


	QMenu* builtInScriptsMenu = ui.menuScripting->addMenu(tr("&Built in scripts"));
	QString script;
	foreach(script,Scripter::getInstance().loadedScripts()){
		QAction* action = new QAction(script,this);
		builtInScriptsMenu->addAction(action);
		connect(action,SIGNAL(triggered()),&m_builtInScriptsActionsMapper,SLOT(map()));
		m_builtInScriptsActionsMapper.setMapping(action,script);
	}
	connect(&m_builtInScriptsActionsMapper,SIGNAL(mapped(QString)),this,SLOT(builtInScriptTriggered(QString)));

        add_calculator_object(&Workspace::getInstace());
	mruToFileMenu();

        connect(&PJobRunnerPool::instance(), SIGNAL(found_new_pjob_runner(QHostAddress)), this, SLOT(found_new_pjob_runner(QHostAddress)));
        connect(&PJobRunnerPool::instance(), SIGNAL(lost_pjob_runner(QHostAddress)), this, SLOT(lost_pjob_runner(QHostAddress)));
        connect(&PJobRunnerPool::instance(), SIGNAL(probing_host(QHostAddress)), this, SLOT(probing_host(QHostAddress)));
        connect(&PJobRunnerPool::instance(), SIGNAL(network_scan_finished()), this, SLOT(pjob_runner_search_finished()));
        connect(&PJobRunnerPool::instance(), SIGNAL(network_scan_started()), this, SLOT(pjob_runner_search_started()));

        foreach(QNetworkInterface i, QNetworkInterface::allInterfaces()){
            ui.networkInterfaceComboBox->addItem(i.humanReadableName());
        }
}

void MainWindow::on_actionOpen_triggered(){
    QString filename = QFileDialog::getOpenFileName(this, "Open PJOB file", "", "PJOB files (*.pjob)");
    if(filename.isEmpty()) return;
    open_pjob_file(filename);
}

void MainWindow::open_pjob_file(QString filename){
    if(m_pjob_file) delete m_pjob_file;
    m_pjob_file = new PJobFile(filename);
    pjobFile_changed();
    mostRecentlyUsedPJOBFilesAdd(filename);
}


void MainWindow::builtInScriptTriggered(const QString& script){
	Scripter::getInstance().runLoadedScript(script);
}

QScriptValue MainWindow::userInputForScript(QScriptContext *context, QScriptEngine *engine){
	emit needScriptedUserInputDialog(context,engine);
	scriptedUserInputDialogMutex.lock();
	scriptedUserInputDialogWaitCondition.wait(&scriptedUserInputDialogMutex);
	scriptedUserInputDialogMutex.unlock();

	return scriptedUserInputDialogResult;
}

void MainWindow::openScriptedUserInputDialog(QScriptContext *context, QScriptEngine *engine){
	ScriptedUserInputDialog dialog(context,engine);
	scriptedUserInputDialogResult = dialog.getValues();
	scriptedUserInputDialogWaitCondition.wakeOne();
}



void MainWindow::on_addParameterButton_clicked(){
	QTableWidgetItem* name = new QTableWidgetItem();
	QTableWidgetItem* value = new QTableWidgetItem();
	//m_parameters.push_back(item);

	int row = ui.parametersWidget->rowCount();
	ui.parametersWidget->setRowCount(row+1);
	ui.parametersWidget->setItem(row,0,name);
	ui.parametersWidget->setItem(row,1,value);
	//ui.parametersWidget->setItem (item,0, new QLineEdit(ui.parametersWidget));
	//ui.parametersWidget->setItemWidget(item,1, new QLineEdit(ui.parametersWidget));
}


void MainWindow::on_delParameterButton_clicked(){
	if(ui.parametersWidget->selectedItems().size() <= 0) return;
	int row = ui.parametersWidget->selectionModel()->selectedRows().first().row();
//	QTableWidgetItem* name = ui.parametersWidget->item(row,0);
//	QTableWidgetItem* value = ui.parametersWidget->item(row,1);
//	delete name;
//	delete value;
	
	for(int i=row+1;i<ui.parametersWidget->rowCount();++i){
		ui.parametersWidget->setItem(i-1,0,new QTableWidgetItem(ui.parametersWidget->item(i,0)->text()));
		ui.parametersWidget->setItem(i-1,1,new QTableWidgetItem(ui.parametersWidget->item(i,1)->text()));
	}
	ui.parametersWidget->setRowCount(ui.parametersWidget->rowCount()-1);
}


void MainWindow::pjobFile_changed(){
        Workspace::getInstace().setPJobFile(m_pjob_file);
	ui.parametersWidget->clear();
	QStringList l1;
	l1 << "Parametername" << "Parametervalue";
	ui.parametersWidget->setColumnCount(2);
	ui.parametersWidget->setHorizontalHeaderLabels(l1);
	ui.parametersWidget->setRowCount(0);

        ui.addJobButton->setEnabled(m_pjob_file != 0);
        if(!m_pjob_file) return;
        QList<PJobFileParameterDefinition> params = m_pjob_file->parameterDefinitions();
	PJobFileParameterDefinition p;
	foreach(p,params){
		QTableWidgetItem* nameItem = new QTableWidgetItem();
		QTableWidgetItem* valueItem = new QTableWidgetItem();
		nameItem->setText(p.name());
		valueItem->setText(QString("%1").arg(p.defaultValue()));

		int row = ui.parametersWidget->rowCount();
		ui.parametersWidget->setRowCount(row+1);
		ui.parametersWidget->setItem(row,0,nameItem);
		ui.parametersWidget->setItem(row,1,valueItem);
	}
}

void MainWindow::on_addJobButton_clicked(){
	QHash<QString,QString> parameters;
	for(int i=0;i<ui.parametersWidget->rowCount();++i){
		QString name = ui.parametersWidget->item(i,0)->text();
		QString value = ui.parametersWidget->item(i,1)->text();
		parameters[name]  = value;
	}

        Workspace::getInstace().addJob(new Job(parameters, &Workspace::getInstace()));
}

void MainWindow::on_jobUpButton_clicked(){
	int selectedRow = ui.jobsWidget->selectionModel()->selectedRows().first().row();
	QListWidgetItem* item = ui.jobsWidget->item(selectedRow);
	Job* job = m_jobs[item];
        Workspace::getInstace().setQueuePosition(job,selectedRow-1);
}

void MainWindow::on_jobDownButton_clicked(){
	int selectedRow = ui.jobsWidget->selectionModel()->selectedRows().first().row();
	QListWidgetItem* item = ui.jobsWidget->item(selectedRow);
	Job* job = m_jobs[item];
        Workspace::getInstace().setQueuePosition(job,selectedRow+1);
}

void MainWindow::on_jobDeleteButton_clicked(){
	QModelIndex index;
	QList<Job*> jobs;
	foreach(index,ui.jobsWidget->selectionModel()->selectedRows()){
		int row = index.row();
		QListWidgetItem* item = ui.jobsWidget->item(row);
		Job* job = m_jobs[item];
		jobs << job;
	}
	Job* job;
	foreach(job,jobs){
                Workspace::getInstace().removeJob(job);
		delete job;
	}
}


void MainWindow::jobCreated(Job* j, unsigned int){
	QListWidgetItem* item = new QListWidgetItem(ui.jobsWidget);
        item->setText(j->description());
	item->setData(Qt::DecorationRole, QColor("silver"));
	m_jobs[item] = j;
        connect(j,SIGNAL(stateChanged(Job*,Job::State)),this,SLOT(jobStateChanged(Job*,Job::State)),Qt::QueuedConnection);
        connect(j,SIGNAL(std_out(QString)),this,SLOT(jobOutput(QString)),Qt::QueuedConnection);
}

void MainWindow::jobRemoved(Job* job){
	for(int row=0;row<ui.jobsWidget->count();++row){
		QListWidgetItem* item = ui.jobsWidget->item(row);
		Job* j = m_jobs[item];
		if(j == job){
			ui.jobsWidget->removeItemWidget(item);
			delete item;
		}
	}
        JobOutputWidget* widget=0;
        foreach(JobOutputWidget* w, m_job_output_widgets){
            if(w->job() == job) widget = w;
        }
        if(widget == 0) return;
        m_job_output_widgets.removeOne(widget);
        delete widget;
}

QListWidgetItem* MainWindow::itemForJob(Job* j){
	QListWidgetItem* item;
	foreach(item, m_jobs.keys()){
		if(m_jobs[item] == j){
			return item;
		}
	}
	return 0;
}

void MainWindow::jobMoved(Job* j, unsigned int position){
	QListWidgetItem* item = itemForJob(j);
	int oldPosition = ui.jobsWidget->row(item);
	ui.jobsWidget->takeItem(oldPosition);
	ui.jobsWidget->insertItem(position,item);
	ui.jobsWidget->setItemSelected(item,true);
}

void MainWindow::updateButtons(){
        ui.startButton->setEnabled(!Workspace::getInstace().isRunning() && ui.jobsWidget->count());
        ui.stopButton->setEnabled(Workspace::getInstace().isRunning());
	ui.jobUpButton->setEnabled(false);
	ui.jobDownButton->setEnabled(false);
	ui.jobDeleteButton->setEnabled(false);
	if(ui.jobsWidget->selectionModel()->selectedRows().isEmpty()) return;
	ui.jobDeleteButton->setEnabled(true);
	ui.jobUpButton->setEnabled(ui.jobsWidget->selectionModel()->selectedRows().first().row()>0);
	ui.jobDownButton->setEnabled(ui.jobsWidget->selectionModel()->selectedRows().first().row()<ui.jobsWidget->count()-1);
}

void MainWindow::jobStateChanged(Job* j, Job::State state){
	QListWidgetItem* item = itemForJob(j);
	switch(state){
		case Job::FINISHED:
			item->setData(Qt::DecorationRole, QColor("lime"));
			break;
		case Job::SUBMITED:
			item->setData(Qt::DecorationRole, QColor("yellow"));
			break;
		case Job::RUNNING:
			item->setData(Qt::DecorationRole, QColor("yellowgreen"));
			break;
		case Job::QUEUED:
			item->setData(Qt::DecorationRole, QColor("silver"));
			break;
		case Job::FAILED:
			item->setData(Qt::DecorationRole, QColor("red"));
			break;
	}
}

void MainWindow::jobOutput(QString s){
    QObject *sender_object = sender();
    Job* job = dynamic_cast<Job*>(sender_object);
    if(!job) return;
    QListWidgetItem* item = itemForJob(job);
    QStringList lines = s.split("\n");
    while(lines.size() > 3) lines.pop_front();
    item->setText(lines.join("\n"));
}

void MainWindow::on_startButton_clicked(){
        Workspace::getInstace().start();
}

void MainWindow::on_stopButton_clicked(){
        Workspace::getInstace().stop();
}

void MainWindow::started(){
	updateButtons();
}

void MainWindow::stopped(){
	updateButtons();
}

void MainWindow::updatePJobFileSelector(QString pjobFile){
	if(ui.pjobFileSelector->findText(pjobFile) != -1) return;
	ui.pjobFileSelector->addItem(pjobFile);
}

void MainWindow::initialSortResultView(){
	ui.resultView->sortByColumn(0, Qt::AscendingOrder);
}
void MainWindow::newValue(){
	if(!ui.navigatorResultsTreeWidget->selectedItems().empty())
		on_navigatorResultsTreeWidget_itemClicked(ui.navigatorResultsTreeWidget->selectedItems().first());
}

void MainWindow::newValue(QString phoFile, QString result, QHash<QString,double>, double){
	QTreeWidgetItem* fileItemNavigator;
	if(m_phoFileItemsNavigator.count(phoFile))
		fileItemNavigator = m_phoFileItemsNavigator[phoFile];
	else{
		fileItemNavigator = new QTreeWidgetItem(ui.navigatorResultsTreeWidget);
		fileItemNavigator->setText(0,phoFile);
		m_phoFileItemsNavigator[phoFile] = fileItemNavigator;
	}

	QTreeWidgetItem* resultItemNavigator;
	if(m_resultItemsNavigator[fileItemNavigator].count(result))
		resultItemNavigator = m_resultItemsNavigator[fileItemNavigator][result];
	else{
		resultItemNavigator = new QTreeWidgetItem(fileItemNavigator);
		resultItemNavigator->setText(0,result);
		m_resultItemsNavigator[fileItemNavigator][result] = resultItemNavigator;
	}
}

void MainWindow::on_actionEdit_triggered(){
	SettingsDialog d;
	d.exec();
}

void MainWindow::on_actionExport_To_CSV_triggered()
{
	QString file = QFileDialog::getSaveFileName(this, tr("Select file for CSV export"),"","*.txt");
	
	//Abbruch, falls der Dialog geschlossen wird
	if(file == NULL) 
		return;

        Workspace::getInstace().getResults().exportToCSV(file,Workspace::getInstace().getResults().phoFiles());
}

void MainWindow::on_actionImport_From_CSV_triggered()
{
	QString file = QFileDialog::getOpenFileName(this, tr("Select file for CSV import"),"","*.txt");
	
	//Abbruch, falls der Dialog geschlossen wird
	if(file == NULL) 
		return;

        QHash< QString,QHash< QHash<QString,double>,QHash<QString,double> > > import = Workspace::getInstace().getResults().importFromCSV(file);
	foreach(QString job,import.keys())
		emit Logger::getInstance().jobResults(import[job],job);
}

void MainWindow::on_actionImport_From_PJob_triggered()
{
	QString file = QFileDialog::getOpenFileName(this, tr("Select file for PJob import"),"","*.pjob");
	
	//Abbruch, falls der Dialog geschlossen wird
	if(file == NULL) 
		return;
	
        Workspace::getInstace().import_results_from_pjobfile(file);
}

void MainWindow::on_navigatorResultsTreeWidget_itemClicked(QTreeWidgetItem* item){
	if(!m_phoFileItemsNavigator.values().count(item)){
		QString result = item->text(0);
		QString phoFile = item->parent()->text(0);
		visualizerSelectResult(phoFile,result);
	}
}


void MainWindow::visualizerSelectResult(QString pjob_file, QString result){
	fillParametersBox(pjob_file);
	updateParametersBox();
	showVisualization(pjob_file, result);
}


void MainWindow::fillParametersBox(QString phoFile){
        QSet<QString> set = Workspace::getInstace().getResults().parametersFor(phoFile);
	QList<QString> parameters = QList<QString>::fromSet(set);
	qSort(parameters);

	bool weHaveFormerValues=false;
	QHash<QString,double> formerSliderValues;
	int formerXAchsis, formerYAchsis;
	if(m_parameterSliders.size()){
		QString p;
		int i=0;
		foreach(p,parameters){
			formerSliderValues[p] = m_parameterSliders.at(i)->value();
			++i;
		}
		weHaveFormerValues = true;
		formerXAchsis = m_x_axis_button_group.checkedId();
		formerYAchsis = m_y_axis_button_group.checkedId();
	}

	QFrame* s;
	foreach(s,m_parameterFrames) delete s;
	m_parameterFrames.clear();
	m_parameterSliders.clear();

	if(parameters.size() < 3) return;

	ui.parametersBox->setLayout(new QVBoxLayout);
	int i=0;
	QString parameter;
	foreach(parameter,parameters){
		QVBoxLayout *verticalLayout = new QVBoxLayout;
		QFrame *frame = new QFrame;
		frame->setLayout(verticalLayout);

		QHBoxLayout *horizontalLayout = new QHBoxLayout;
		QFrame *innerframe = new QFrame;
		innerframe->setLayout(horizontalLayout);
		QLabel* label = new QLabel(QString("%1:").arg(parameter),frame);
		horizontalLayout->addWidget(label);

		QRadioButton *radio_x = new QRadioButton("x", innerframe);
		QRadioButton *radio_y = new QRadioButton("y", innerframe);
		m_x_axis_button_group.addButton(radio_x,i);
		m_y_axis_button_group.addButton(radio_y,i);
		horizontalLayout->addWidget(radio_x);
		horizontalLayout->addWidget(radio_y);

		verticalLayout->addWidget(innerframe);
		NonEquidistantSlider* slider = new NonEquidistantSlider(frame);
		m_parameterSliders.append(slider);
		connect(slider,SIGNAL(valueChanged(double)),this,SLOT(showVisualization()));
		verticalLayout->addWidget(slider);
		ui.parametersBox->layout()->addWidget(frame);
		m_parameterFrames.append(frame);

                QList<double> values = Workspace::getInstace().getResults().valuesFor(phoFile,parameter);
		slider->setValues(values);
		++i;
	}

	if(weHaveFormerValues){
		m_x_axis_button_group.button(formerXAchsis)->setChecked(true);
		m_y_axis_button_group.button(formerYAchsis)->setChecked(true);
                QSet<QString> set = Workspace::getInstace().getResults().parametersFor(phoFile);
		QList<QString> parameters = QList<QString>::fromSet(set);
		qSort(parameters);
		int psize = parameters.size();
		int ssize = m_parameterSliders.size();
		int fsize = formerSliderValues.size();
		assert(psize == ssize);
		assert(fsize == psize);
		int i=0;
		NonEquidistantSlider* slider;
		foreach(slider,m_parameterSliders){
			QString p = parameters.at(i);
			double d = formerSliderValues[p];
			slider->setValue(d);
			++i;
		}	
	}else{
		//Bevor die Buttons aktiviert werden können muss (indirekt) überprüft werden, ob überhaupt welche erzeugt wurden!
		if(parameters.size() == 1)
		{
			m_x_axis_button_group.button(0)->setChecked(true);
		}
		else if(parameters.size() >= 2)
		{
			m_x_axis_button_group.button(0)->setChecked(true);
			m_y_axis_button_group.button(1)->setChecked(true);
		}
	}
}

void MainWindow::updateParametersBox(){
	QAbstractButton* b;
	foreach(b,m_x_axis_button_group.buttons())
		b->setEnabled(true);
	foreach(b,m_y_axis_button_group.buttons())
		b->setEnabled(true);
	NonEquidistantSlider* s;
	foreach(s,m_parameterSliders)
		s->setEnabled(true);

	int xaxis = m_x_axis_button_group.checkedId();
	int yaxis = m_y_axis_button_group.checkedId();
	if(xaxis==-1 || yaxis==-1) return;
	m_parameterSliders.at(xaxis)->setEnabled(false);
	m_parameterSliders.at(yaxis)->setEnabled(false);
	m_y_axis_button_group.button(xaxis)->setEnabled(false);
	m_x_axis_button_group.button(yaxis)->setEnabled(false);
}

void MainWindow::showVisualization(){
	QList<QTreeWidgetItem*> selectedItems = ui.navigatorResultsTreeWidget->selectedItems();
	assert(!selectedItems.empty());
	QTreeWidgetItem* selected = selectedItems.first();
	QTreeWidgetItem* parent = selected->parent();
	assert(parent);
	QString pjob_file = parent->text(0);
	QString result = selected->text(0);
	showVisualization(pjob_file,result);
}

void MainWindow::showVisualization(QString pjob_file, QString result)
{
        QList<QString> parameters = QList<QString>::fromSet(Workspace::getInstace().getResults().parametersFor(pjob_file));
	qSort(parameters);

	QString xachsis = "";
	QString yachsis = "";
	QHash<QString,double> parameterCombination;

	if(parameters.size() == 1){
		xachsis = yachsis = parameters.front();
	}else if(parameters.size() == 2){
		xachsis = parameters.front();
		yachsis = parameters.back();
	}else{
		//Überprüfen, ob überhaupt ein Button angewählt wurde (sonst Absturz!)
		int id = m_x_axis_button_group.checkedId();
		if(id != -1)
			xachsis = parameters.at(id);

		id = m_y_axis_button_group.checkedId();
		if(id != -1)
			yachsis = parameters.at(id);

		QString p;
		int i=0;
		foreach(p,parameters){
			parameterCombination[p] = m_parameterSliders.at(i)->value();
			++i;
		}
	}

	m_plotWidget->show(pjob_file,result,parameterCombination,xachsis,yachsis);
}

void MainWindow::on_scriptLineEdit_returnPressed(){
	QString script = ui.scriptLineEdit->text();
	ui.scriptLineEdit->setText("");

	ui.scriptOutput->insertPlainText(">> " + script + "\n");

	Scripter::getInstance().run(script);
}


void MainWindow::on_actionLoad_script_from_file_triggered(){
	QStringList files = QFileDialog::getOpenFileNames(this,"Select PQueue script file",
		"","JavaScript file (*.js)");
	if(files.isEmpty())return;
	Scripter::getInstance().loadScriptFile(files.first());
}


void MainWindow::addLoadedScriptToListWidget(QString filename){
	QListWidgetItem* item = new QListWidgetItem;
	item->setText(filename);
	ui.scriptsListWidget->addItem(item);
}

void MainWindow::on_runScriptButton_clicked(){
	int row = ui.scriptsListWidget->selectionModel()->selectedRows().first().row();
	QListWidgetItem* item = ui.scriptsListWidget->item(row);
	Scripter::getInstance().runLoadedScript(item->text());
}

void MainWindow::on_stopScriptButton_clicked(){
	Scripter::getInstance().stop(m_runningScript);
}

void MainWindow::scriptStarted(QString engineName){
	ui.scriptLineEdit->setEnabled(false);
	ui.scriptsListWidget->setEnabled(false);
	ui.scriptProgressBar->setHidden(false);
	if(engineName=="console") return;
	ui.scriptOutput->moveCursor(QTextCursor::End);
	ui.scriptOutput->append(QString("Starting script '%1':\n--------------------------\n").arg(engineName));
	ui.runScriptButton->setEnabled(false);
	ui.stopScriptButton->setEnabled(true);
	m_runningScript = engineName;
}

void MainWindow::scriptFinished(QString engineName, QString output){
	ui.scriptLineEdit->setEnabled(true);
	ui.scriptsListWidget->setEnabled(true);
	ui.scriptProgressBar->setHidden(true);
	if(engineName!="console"){
		ui.scriptOutput->moveCursor(QTextCursor::End);
		ui.scriptOutput->append(
			QString("Finished script '%1'!\nOutput: %2\n--------------------------\n").arg(engineName).arg(output)
			);
		ui.runScriptButton->setEnabled(true);
		ui.stopScriptButton->setEnabled(false);
	}else
		ui.scriptOutput->insertPlainText(output+"\n");
	/*QString row;
	foreach(row,output.split("\n")){
		QListWidgetItem* item = new QListWidgetItem;
		item->setText(row);
		ui.scriptOutput->addItem(item);
		ui.scriptOutput->scrollToItem(item);
	}
	*/
}


void MainWindow::setScriptProgress(QString, unsigned int progress){
	ui.scriptProgressBar->setMinimum(0);
	ui.scriptProgressBar->setMaximum(100);
	ui.scriptProgressBar->setValue(progress);
}


QStringList MainWindow::mostRecentlyUsedPJOBFiles(){
	QStringList mru;
	QSettings settings("HFT", "PQueue");
	settings.beginGroup("gui");
	mru << settings.value("mruPJOBFile1","").toString();
	mru << settings.value("mruPJOBFile2","").toString();
	mru << settings.value("mruPJOBFile3","").toString();
	settings.endGroup();
	return mru;
}

void MainWindow::mostRecentlyUsedPJOBFilesAdd(QString pjobFilePath){
	QStringList mru = mostRecentlyUsedPJOBFiles();
	mru.prepend(pjobFilePath);
	mru.pop_back();
	QSettings settings("HFT", "PQueue");
	settings.beginGroup("gui");
	settings.setValue("mruPJOBFile1", mru.at(0));
	settings.setValue("mruPJOBFile2", mru.at(1));
	settings.setValue("mruPJOBFile3", mru.at(2));
	settings.endGroup();
	mruToFileMenu();
}

void MainWindow::mruToFileMenu(){
	QStringList mru = mostRecentlyUsedPJOBFiles();
	if(mru.at(0)!=""){
		ui.actionMruAction1->setVisible(true);
		ui.actionMruAction1->setText(mru.at(0));
	}else ui.actionMruAction1->setVisible(false);
	if(mru.at(1)!=""){
		ui.actionMruAction2->setVisible(true);
		ui.actionMruAction2->setText(mru.at(1));
	}else ui.actionMruAction2->setVisible(false);
	if(mru.at(2)!=""){
		ui.actionMruAction3->setVisible(true);
		ui.actionMruAction3->setText(mru.at(2));
	}else ui.actionMruAction3->setVisible(false);
}

void MainWindow::on_actionMruAction1_triggered(){
    open_pjob_file(mostRecentlyUsedPJOBFiles().at(0));
}

void MainWindow::on_actionMruAction2_triggered(){
    open_pjob_file(mostRecentlyUsedPJOBFiles().at(1));
}

void MainWindow::on_actionMruAction3_triggered(){
    open_pjob_file(mostRecentlyUsedPJOBFiles().at(2));
}

void MainWindow::on_loadPreviousRunsButton_clicked()
{
	//redundante Funktion bereits implementiert im Result-Menü
	on_actionImport_From_PJob_triggered();
}

extern QSplashScreen* global_splash_screen;

void MainWindow::hide_splash_screen(){
    global_splash_screen->finish(this);
    delete global_splash_screen;
    global_splash_screen = 0;
}


void MainWindow::viewResult(QString pjob_file, QString result){
	ui.tabWidget->setCurrentIndex(2);
	visualizerSelectResult(pjob_file,result);
}

void MainWindow::found_new_pjob_runner(QHostAddress host){
    if(m_pjob_runner_items.contains(host)) return;

    QString hostname = PJobRunnerPool::instance().hostname(host);
    QString os = PJobRunnerPool::instance().platform(host);
    unsigned int thread_count = PJobRunnerPool::instance().max_thread_count_for_host(host);
    QListWidgetItem* item = new QListWidgetItem();
    item->setData(Qt::DecorationRole, QColor("lime"));
    item->setData(Qt::DisplayRole, QString("%1 (%2) with %4 threads(s) running %3").arg(hostname).arg(host.toString()).arg(os).arg(thread_count));
    m_pjob_runner_items[host] = item;
    ui.pjobRunnerListWidget->addItem(item);
    if(hostname.isEmpty()) QHostInfo::lookupHost(host.toString(), this, SLOT(lookedUp(QHostInfo)));
}

void MainWindow::lookedUp(const QHostInfo& host){
    assert(!host.addresses().isEmpty());
    QHostAddress address = host.addresses().first();
    assert(m_pjob_runner_items.contains(address));
    QString hostname = PJobRunnerPool::instance().hostname(address);
    QString os = PJobRunnerPool::instance().platform(address);
    unsigned int thread_count = PJobRunnerPool::instance().max_thread_count_for_host(address);
    m_pjob_runner_items[address]->setData(Qt::DisplayRole, QString("%1 (%2) with %4 threads(s) running %3").arg(hostname).arg(address.toString()).arg(os).arg(thread_count));
}

void MainWindow::lost_pjob_runner(QHostAddress host){
    if(!m_pjob_runner_items.contains(host)) return;
    delete m_pjob_runner_items.take(host);
}

void MainWindow::probing_host(QHostAddress host){
    ui.poolStatusLabel->setText(QString("Probing %1 ...").arg(host.toString()));
}

void MainWindow::pjob_runner_search_finished(){
    ui.poolStatusLabel->setText(QString("Search finished."));
    ui.startScanButton->setText("Start scan");
}

void MainWindow::pjob_runner_search_started(){
    ui.startScanButton->setText("Stop scan");
}

void MainWindow::on_jobsWidget_itemDoubleClicked(QListWidgetItem* item){
    Job* job = m_jobs[item];
    foreach(JobOutputWidget* w, m_job_output_widgets){
        if(w->job() == job){
            w->show();
            return;
        }
    }
    JobOutputWidget *w = new JobOutputWidget(job,0);
    m_job_output_widgets.append(w);
    w->show();
}

void MainWindow::on_startScanButton_clicked(){
    if(PJobRunnerPool::instance().is_scanning()){
        PJobRunnerPool::instance().stop_search_network();
        return;
    }
    QString interface_name = ui.networkInterfaceComboBox->currentText();
    if(interface_name.isEmpty()) return;
    foreach(QNetworkInterface i, QNetworkInterface::allInterfaces()){
        if(i.humanReadableName() == interface_name)
            PJobRunnerPool::instance().start_search_network(i);
    }
}
