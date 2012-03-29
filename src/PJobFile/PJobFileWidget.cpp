#include "PJobFileWidget.h"
#include "PJobFile.h"
#include "FileHelperFunctions.h"
#include <QtGui/QFileDialog>
#include <QtGui/QMenu>
#include <QtCore/QUrl>
#include <QtCore/QMimeData>
#include <QtGui/QComboBox>
#include "SyntaxHighlighter.h"

PJobFileWidget::PJobFileWidget(PJobFile* jobFile, QWidget* parent )
: QWidget(parent), m_pjobFile(jobFile)
{
	ui.setupUi(this);

	//
	// Resources Verzeichnis Model/ View initialisieren
	//
	m_pjobDirModel = new PJobDirModel(m_pjobFile->getPJobFile(), "Resources", this);
	ui.resourcesTreeView->setModel(m_pjobDirModel);
	ui.resourcesTreeView->setColumnWidth(0, 300);
	ui.resourcesTreeView->setColumnWidth(1, 180);
	ui.resourcesTreeView->setAcceptDrops(true);
	ui.resourcesTreeView->setDragEnabled(true);
	ui.resourcesTreeView->setDropIndicatorShown(false);
	ui.resourcesTreeView->setDragDropMode(QAbstractItemView::DragDrop);
	ui.resourcesTreeView->setDragDropOverwriteMode(true);
	ui.resourcesTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
	connect(m_pjobDirModel, SIGNAL(changed()), this, SLOT(resourcesChanged()));

	// Resources Rechtsklick/ Context Menü initialisieren
	QAction* actionAddResources = new QAction("Add resources...", this);
	QAction* actionMakeDir = new QAction("Make directory", this);
	QAction* actionExtract = new QAction("Extract to...", this);
	QAction* actionRename = new QAction("Rename", this);
	QAction* actionDelete = new QAction("Delete", this);
	QAction* separator = new QAction(this);
	separator->setSeparator(true);
	m_resourcesGeneralActions << actionAddResources << actionMakeDir;
	m_resourcesFileActions << actionExtract << actionRename << actionDelete << separator << m_resourcesGeneralActions;	
	connect(actionAddResources, SIGNAL(triggered()), this, SLOT(onResourcesActionAddResources()));
	connect(actionMakeDir, SIGNAL(triggered()), this, SLOT(onResourcesActionMakeDir()));
	connect(actionExtract, SIGNAL(triggered()), this, SLOT(onResourcesActionExtract()));
	connect(actionRename, SIGNAL(triggered()), this, SLOT(onResourcesActionRename()));
	connect(actionDelete, SIGNAL(triggered()), this, SLOT(onResourcesActionDelete()));
	ui.resourcesTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui.resourcesTreeView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showResourcesContextMenu(const QPoint&)));

	//
	// Runs Verzeichnis Model/ View initialisieren
	//
	m_runDirModel = new PJobDirModel(m_pjobFile->getPJobFile(), "Runs", this);
	ui.runsTreeView->setModel(m_runDirModel);
	ui.runsTreeView->setColumnWidth(0, 300);
	ui.runsTreeView->setColumnWidth(1, 180);
	ui.runsTreeView->setDragEnabled(true);
	ui.runsTreeView->setDragDropMode(QAbstractItemView::DragOnly);
	ui.runsTreeView->setDropIndicatorShown(false);
	ui.runsTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);

        SyntaxHighlighter* syntaxHighlighter = new SyntaxHighlighter(ui.scriptTextEdit->document());
	ui.scriptTextEdit->setText(m_pjobFile->mainPscript());
	connect(ui.scriptTextEdit, SIGNAL(textChanged()), this, SLOT(mainScriptChanged()));

	syncParametersFileToModel();
	ui.parametersTableView->setModel(&m_parametersModel);
	QStringList parametersTableViewHeadersLables;
	parametersTableViewHeadersLables << "Name" << "Default Value" << "Unit" << "Min" << "Max";
	m_parametersModel.setHorizontalHeaderLabels(parametersTableViewHeadersLables);
	connect(&m_parametersModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(parametersChanged()));

	syncResultsFileToModel();
	ui.resultsTreeView->setModel(&m_resultsModel);
        m_resultsDelegate = new ResultsTypeDelegate(this);
        ui.resultsTreeView->setItemDelegate(m_resultsDelegate);
	QStringList resultsTreeViewHeadersLables;
	resultsTreeViewHeadersLables << "File/Result" << "File format" << "Unit";
	m_resultsModel.setHorizontalHeaderLabels(resultsTreeViewHeadersLables);
        ui.resultsTreeView->setColumnWidth(0,250);
        ui.resultsTreeView->setColumnWidth(1,150);
	connect(&m_resultsModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(resultsChanged()));

    QFont font("Bitstream Vera Sans Mono",10);
    ui.scriptTextEdit->setFont(font);

    QList<PJobFileBinary> binaries = m_pjobFile->binaries();
    foreach(PJobFileBinary b, binaries){
            ui.binariesComboBox->addItem(b.name);
    }
}

void PJobFileWidget::refresh(){
	if(m_runDirModel) delete m_runDirModel;
	m_runDirModel = new PJobDirModel(m_pjobFile->getPJobFile(), "Runs", this);
	ui.runsTreeView->setModel(m_runDirModel);
	//ui.runsTreeView->reset();
}


void PJobFileWidget::mainScriptChanged(){
	m_pjobFile->setMainPscript(ui.scriptTextEdit->toPlainText());
	emit changed();
}

void PJobFileWidget::showResourcesContextMenu(const QPoint& pos){
	m_clickedIndex = ui.resourcesTreeView->indexAt(pos);
	if(ui.resourcesTreeView->indexAt(pos).isValid())
		QMenu::exec(m_resourcesFileActions, QCursor::pos());
	else QMenu::exec(m_resourcesGeneralActions, QCursor::pos());
}

void PJobFileWidget::onResourcesActionExtract(){
	QString targetDir = QFileDialog::getExistingDirectory(this, "Extract to...");
	QModelIndex index;
	foreach(index, ui.resourcesTreeView->selectionModel()->selectedIndexes()){
		if(index.column() != 0) continue;
		m_pjobFile->getPJobFile()->extract( targetDir, (static_cast<PJobDirTree*>(index.internalPointer()))->data(0).toString());
	}
}

void PJobFileWidget::onResourcesActionRename(){
        QModelIndex currentIndex(ui.resourcesTreeView->currentIndex());
	ui.resourcesTreeView->edit(m_pjobDirModel->index(currentIndex.row(), 0, currentIndex.parent()));
}

void PJobFileWidget::onResourcesActionDelete(){
	m_pjobDirModel->removeEntries(ui.resourcesTreeView->selectionModel()->selectedIndexes());
}

void PJobFileWidget::onResourcesActionMakeDir(){
	ui.resourcesTreeView->setCurrentIndex( m_pjobDirModel->makeDir(m_clickedIndex) );
	ui.resourcesTreeView->edit( ui.resourcesTreeView->currentIndex() );
}

void PJobFileWidget::onResourcesActionAddResources(){
	QStringList files = QFileDialog::getOpenFileNames(this, "Select resources to add...");
	QList<QUrl> urls;
	QString filePath;
	foreach(filePath, files){
		urls << QUrl("file:///" + filePath);
	}
	QMimeData* mimeData = new QMimeData();
	mimeData->setUrls(urls);
	m_pjobDirModel->dropMimeData(mimeData, Qt::CopyAction, 0, 0, m_clickedIndex);
}

void PJobFileWidget::resourcesChanged(){
	emit changed();
}

void PJobFileWidget::parametersChanged(){
	syncParametersModelToFile();
	emit changed();
}

void PJobFileWidget::resultsChanged(){
	syncResultsModelToFile();
	emit changed();
}

void PJobFileWidget::on_parametersAddButton_clicked(){
	m_parametersModel.setRowCount(m_parametersModel.rowCount()+1);
	int row = m_parametersModel.rowCount()-1;
	disconnect(&m_parametersModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(parametersChanged()));
	m_parametersModel.setItem(row, 0, new QStandardItem("New Parameters"));
	m_parametersModel.setItem(row, 1, new QStandardItem("0"));
	m_parametersModel.setItem(row, 2, new QStandardItem(""));
	m_parametersModel.setItem(row, 3, new QStandardItem(""));
	m_parametersModel.setItem(row, 4, new QStandardItem(""));
	parametersChanged();
	connect(&m_parametersModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(parametersChanged()));
}

void PJobFileWidget::on_parametersRemoveButton_clicked(){
	int row = ui.parametersTableView->selectionModel()->currentIndex().row();
	if(row >= 0){
		m_parametersModel.removeRow(row);
		parametersChanged();
	}
}

void PJobFileWidget::on_addResultFileButton_clicked(){
	disconnect(&m_resultsModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(resultsChanged()));
	QStandardItem *parentItem = m_resultsModel.invisibleRootItem();
	int row = parentItem->rowCount();
	QStandardItem *fileitem = new QStandardItem("New Resultfile");
        QStandardItem *formatitem = new QStandardItem("SINGLE_VALUE");
	QStandardItem *placeHolder1 = new QStandardItem("");
	placeHolder1->setEditable(false);
	parentItem->setChild(row, 0, fileitem);
	parentItem->setChild(row, 1, formatitem);
	parentItem->setChild(row, 2, placeHolder1);
	resultsChanged();
	connect(&m_resultsModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(resultsChanged()));
}

void PJobFileWidget::on_removeResultFileButton_clicked(){
	disconnect(&m_resultsModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(resultsChanged()));
	QStandardItem* item = m_resultsModel.itemFromIndex(ui.resultsTreeView->selectionModel()->currentIndex());
	if(item->parent() != m_resultsModel.invisibleRootItem() && item->parent() != 0)
		item = item->parent();
	m_resultsModel.removeRow(item->row());
	resultsChanged();
	connect(&m_resultsModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(resultsChanged()));
}

void PJobFileWidget::on_addResultButton_clicked(){
	disconnect(&m_resultsModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(resultsChanged()));
	// Index des Items in der ersten Spalte holen
	QModelIndex itemIndex =ui.resultsTreeView->selectionModel()->currentIndex();
	QStandardItem* item = m_resultsModel.itemFromIndex(m_resultsModel.index(itemIndex.row(),0,itemIndex.parent()));
	if(item->parent() != m_resultsModel.invisibleRootItem() && item->parent() != 0)
		item = item->parent();

	int resultrow = item->rowCount();
	QStandardItem *resultname= new QStandardItem("New Result");
	item->setChild(resultrow,0,resultname);
	QStandardItem *placeHolder2 = new QStandardItem("");
        placeHolder2->setEditable(false);
	item->setChild(resultrow,1,placeHolder2);
	QStandardItem *resultunit= new QStandardItem("");
	item->setChild(resultrow,2,resultunit);

	resourcesChanged();
	connect(&m_resultsModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(resultsChanged()));
}

void PJobFileWidget::on_removeResultButton_clicked(){
	disconnect(&m_resultsModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(resultsChanged()));
	QStandardItem* item = m_resultsModel.itemFromIndex(ui.resultsTreeView->selectionModel()->currentIndex());
	if(item->parent() != m_resultsModel.invisibleRootItem() && item->parent() != 0){
		QStandardItem* resultfileItem = item->parent();
		resultfileItem->removeRow(item->row());
	}
	resultsChanged();
	connect(&m_resultsModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(resultsChanged()));
}


void PJobFileWidget::syncParametersModelToFile(){
	QList<PJobFileParameterDefinition> definitions;
	for(int row=0; row < m_parametersModel.rowCount(); ++row){
		PJobFileParameterDefinition def;
		def.setName(m_parametersModel.item(row,0)->text());
		def.setDefaultValue(m_parametersModel.item(row,1)->text().toDouble());
		if(m_parametersModel.item(row,2)->text().length())
			def.setUnit(m_parametersModel.item(row,2)->text());
		if(m_parametersModel.item(row,3)->text().length())
			def.setMinValue(m_parametersModel.item(row,3)->text().toDouble());
		if(m_parametersModel.item(row,4)->text().length())
			def.setMaxValue(m_parametersModel.item(row,4)->text().toDouble());
		definitions << def;
	}
	m_pjobFile->writeParameterDefinitions(definitions);
}

void PJobFileWidget::syncParametersFileToModel(){
	QList<PJobFileParameterDefinition> definitions = m_pjobFile->parameterDefinitions();
	m_parametersModel.setRowCount(definitions.size());
	m_parametersModel.setColumnCount(5);
	PJobFileParameterDefinition def;
	foreach(def,definitions){
		int row = definitions.indexOf(def);
		m_parametersModel.setItem(row, 0, new QStandardItem(def.name()));
		m_parametersModel.setItem(row, 1, new QStandardItem(QString("%1").arg(def.defaultValue())));
		m_parametersModel.setItem(row, 2, new QStandardItem(def.hasUnit()?def.unit():""));
		m_parametersModel.setItem(row, 3, new QStandardItem(def.hasMinValue()?QString("%1").arg(def.minValue()):""));
		m_parametersModel.setItem(row, 4, new QStandardItem(def.hasMaxValue()?QString("%1").arg(def.maxValue()):""));
	}
}

void PJobFileWidget::syncResultsModelToFile(){
	QList<PJobResultFile> resultFiles;
	QStandardItem *parentItem = m_resultsModel.invisibleRootItem();
	for(int row=0; row < parentItem->rowCount(); ++row){
		QStandardItem* resultfileitem = parentItem->child(row,0);
		QStandardItem* formatitem = parentItem->child(row,1);
		PJobResultFile resultFile;
		resultFile.setFilename(resultfileitem->text());
                QString format = formatitem->data(Qt::EditRole).toString();
                if(format.contains("SINGLE_VALUE"))
                    resultFile.setType(PJobResultFile::SINGLE_VALUE);
                if(format.contains("CSV"))
                    resultFile.setType(PJobResultFile::CSV);
		for(int resultrow=0; resultrow < resultfileitem->rowCount(); ++resultrow){
			PJobResult result;
			QStandardItem* nameitem = resultfileitem->child(resultrow,0);
			QStandardItem* unititem = resultfileitem->child(resultrow,2);
			result.setName(nameitem->text());
			result.setUnit(unititem->text());
			resultFile.addResult(result);
		}
		resultFiles.append(resultFile);
	}

	m_pjobFile->writeResultDefinitions(resultFiles);
}

void PJobFileWidget::syncResultsFileToModel(){
	QList<PJobResultFile> resultFiles = m_pjobFile->readResultDefinitions();
	QStandardItem *parentItem = m_resultsModel.invisibleRootItem();
	PJobResultFile file;
	int filerow=0;
	foreach(file,resultFiles) {
		QStandardItem *fileitem = new QStandardItem(file.filename());
                QStandardItem *formatitem;
                switch(file.type()){
                case PJobResultFile::SINGLE_VALUE:
                    formatitem= new QStandardItem("SINGLE_VALUE");
                    break;
                case PJobResultFile::CSV:
                    formatitem= new QStandardItem("CSV");
                    break;
                default:
                    Q_ASSERT(false);
                    formatitem= new QStandardItem("");
                }


		QStandardItem *placeHolder1 = new QStandardItem("");
		placeHolder1->setEditable(false);
		parentItem->setChild(filerow, 0, fileitem);
		parentItem->setChild(filerow, 1, formatitem);
		parentItem->setChild(filerow, 2, placeHolder1);
		PJobResult result;
		int resultrow=0;
		foreach(result,file.results()){
			QStandardItem *resultname= new QStandardItem(result.name());
			fileitem->setChild(resultrow,0,resultname);
			QStandardItem *placeHolder2 = new QStandardItem("");
			placeHolder2->setEditable(false);
			fileitem->setChild(resultrow,1,placeHolder2);
			QStandardItem *resultunit= new QStandardItem(result.unit());
			fileitem->setChild(resultrow,2,resultunit);
			++resultrow;
		}
		++filerow;
	}
}


void PJobFileWidget::on_binariesComboBox_currentIndexChanged(int index){

}




ResultsTypeDelegate::ResultsTypeDelegate(QObject *parent)
    : QItemDelegate(parent)
{

}

QWidget *ResultsTypeDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                      const QModelIndex &index) const{

    if(index.column() != 1 || index.parent() != QModelIndex())
        return QItemDelegate::createEditor(parent,option,index);

    QComboBox* editor = new QComboBox(parent);
    editor->addItem("SINGLE_VALUE");
    editor->addItem("CSV");

    return editor;
}

void ResultsTypeDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(index.column() != 1 || index.parent() != QModelIndex())
        return QItemDelegate::setEditorData(editor,index);

    QString value = index.model()->data(index, Qt::EditRole).toString();
    QComboBox* combo_box = static_cast<QComboBox*>(editor);
    if("SINGLE_VALUE" == value)
        combo_box->setCurrentIndex(0);
    if("CSV" == value)
        combo_box->setCurrentIndex(1);
}

void ResultsTypeDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                  const QModelIndex &index) const
{
    if(index.column() != 1 || index.parent() != QModelIndex())
        return QItemDelegate::setModelData(editor,model,index);

    QComboBox* comboBox = static_cast<QComboBox*>(editor);
    QString value;
    switch(comboBox->currentIndex()){
    case 0:
        value = "SINGLE_VALUE";
        break;
    case 1:
        value = "CSV";
        break;
    default:
        Q_ASSERT(false);
    }

    model->setData(index, value, Qt::EditRole);
}


