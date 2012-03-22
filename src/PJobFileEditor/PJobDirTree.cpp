#include "PJobDirTree.h"
#include <QtCore/QStringList>
#include <QtCore/QDateTime>
#include <QtCore/QList>

PJobDirTree::PJobDirTree(PJobDirTree* parent, QList<QVariant> data)
	: m_parent(parent), m_data(data)
{

}

PJobDirTree* PJobDirTree::append(QList<QVariant> data){
	PJobDirTree* item = findItem(data[0]);
	if(item == NULL){
		item = new PJobDirTree;
		this->appendItem(item);
	}
	item->m_data = data;
	return item;
}

void PJobDirTree::appendItem(PJobDirTree* item){
	item->m_parent = this;
	m_children << item;
}

void PJobDirTree::deleteChild(PJobDirTree* item){
	int index = m_children.indexOf(item);
	if(index == -1) return;
	delete m_children.takeAt(index);
}

QVariant PJobDirTree::data(int index){
	if(index < 0 || index >= m_data.size()) return QVariant();
	return m_data[index];
}

void PJobDirTree::setData(int index, QVariant value){
	m_data[index] = value;
}

PJobDirTree* PJobDirTree::parent(){
	return m_parent;
}

PJobDirTree* PJobDirTree::childAt(int row){
	if(row < 0 || row >= m_children.size()) return NULL;
	return m_children[row];
}

int PJobDirTree::childCount(){
	return m_children.size();
}

int PJobDirTree::columnCount(){
	return m_data.size();
}

int PJobDirTree::row(){
	if(m_parent == NULL) return -1;
	return m_parent->m_children.indexOf(this);
}

PJobDirTree* PJobDirTree::findItem(QVariant value){
	PJobDirTree* item;
	foreach(item, m_children)
		if(item->m_data[0] == value) return item;
	return NULL;
}

bool PJobDirTree::isDirectory(){
	if(m_data.size() == 1) return true;
	return false;
}

void PJobDirTree::updatePath(int pos, int n, QString newPath){
	PJobDirTree* item;
	foreach(item, m_children){
		item->updatePath(pos, n, newPath);
	}
	m_data[0] = QVariant(m_data[0].toString().replace(pos, n, newPath));
}

int PJobDirTree::populateTree(QList<QList<QVariant> > fileList){
	if(!this->isDirectory()) return 0;
	int depth = m_data[0].toString().split("/").size();
	return populateTreeRec(fileList, this, m_data[0].toString() + "/", depth);
}

int PJobDirTree::populateTreeRec(QList<QList<QVariant> > fileList, PJobDirTree* currentItem,
								 QString currentDir, int depth, int index)
{
	while(index < fileList.size())
	{
		QStringList splitPath = fileList[index][0].toString().split("/");
		splitPath.removeLast();

		// Wenn das momentane Verzeichnis nicht im Pfad der aktuellen Datei liegt, kehrt die Funktion zurück
		if(!fileList[index][0].toString().startsWith(currentDir)) return index;

		if(depth == splitPath.size())
			currentItem->append(PJobDirTree::formatDataToString(fileList[index++]));
		else if(depth < splitPath.size()){
			QString partDir = currentDir + splitPath[depth];
			PJobDirTree* item = currentItem->findItem(QVariant(partDir));
			if(item == NULL){
				QList<QVariant> data;
				data << QVariant(partDir);
				item = currentItem->append(data);
			}
			index = populateTreeRec(fileList, item, partDir + "/", depth+1, index);
		}
	}
	return index;
}

QList<QVariant> PJobDirTree::formatDataToString(const QList<QVariant>& fileProperties){
		QList<QVariant> dataAsString;

		// Relativen Datei Pfad hinzufügen
		dataAsString << fileProperties[0];

		// Konvertiere Datum
		dataAsString << QVariant( QDateTime::fromTime_t(fileProperties[1].toULongLong()).toString("ddd dd.MM.yyyy hh:mm:ss") );

		// Konvertiere Dateigrößen
		for(int x = 2; x<4; x++){
			double sizeByte = fileProperties[x].toInt();
			if(sizeByte > (1024*1024*1024)) dataAsString << QVariant( QString::number(sizeByte/(1024*1024*1024), 'f', 2) + " GiB" );
			else if(sizeByte > (1024*1024)) dataAsString << QVariant( QString::number(sizeByte/(1024*1024), 'f', 2) + " MiB" );
			else if(sizeByte > 1024) dataAsString << QVariant( QString::number(sizeByte/(1024), 'f', 2) + " KiB" );
			else dataAsString << QVariant( QString::number(sizeByte, 'f', 0) + " Byte" );
		}
		return dataAsString;
}

PJobDirTree::~PJobDirTree(){
	PJobDirTree* item;
	foreach(item, m_children){
		delete item;
	}
}
