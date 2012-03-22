#include "ResultModel.h"
#include <QtGui/QColor>

ResultModel::ResultModel(Results* resultSpace, QObject* parent)
	: m_resultSpace(resultSpace), m_pjobFile(QString("")), QAbstractTableModel(parent)
{
}

QVariant ResultModel::headerData(int section, Qt::Orientation orientation, int role) const{
	if(orientation != Qt::Horizontal || role != Qt::DisplayRole) return QVariant();
	if(section < m_paramNames.size()) return QVariant(m_paramNames.at(section));
	else return QVariant(m_resultNames.at(section - m_paramNames.size()));
}

int ResultModel::rowCount(const QModelIndex &) const{
	return m_paramCombinations.size();
}

int ResultModel::columnCount(const QModelIndex &) const{
	return m_paramNames.size() + m_resultNames.size();
}

QVariant ResultModel::data(const QModelIndex& index, int role) const{
	if(role == Qt::BackgroundRole && index.column() < m_paramNames.size()) return QVariant(QColor(225,225,225));
	if(!index.isValid() || role != Qt::DisplayRole) return QVariant();

	if(index.column() < m_paramNames.size()){
		return QVariant(m_paramCombinations[index.row()][m_paramNames.at(index.column())]);
	}

	return QVariant(m_resultSpace->getValue(m_pjobFile, m_resultNames.at(index.column() - m_paramNames.size()), m_paramCombinations[index.row()]));
}

void ResultModel::initialize(QString pjobFile){
	purge();
	m_pjobFile = pjobFile;
	update(pjobFile);
	emit modelInitialized();
}

void ResultModel::update(QString pjobFile){
	if(pjobFile != m_pjobFile) return;
	emit layoutAboutToBeChanged();
	m_paramNames = m_resultSpace->parametersFor(m_pjobFile).toList();
	m_resultNames = m_resultSpace->resultsFor(m_pjobFile).toList();
	m_paramCombinations = m_resultSpace->parameterCombinationsFor(m_pjobFile).toList();
	emit layoutChanged();
}

void ResultModel::purge(){
	beginResetModel();
	m_pjobFile = "";
	m_paramNames.clear();
	m_resultNames.clear();
	m_paramCombinations.clear();
	endResetModel();
}
