#pragma once

#include <QtCore/QByteArray>
#include <QtXml/QDomDocument>
#include "PJobFileParameter.h"
#include "PJobFileParameterDefinition.h"
#include "PJobResultFile.h"
#include "PJobFileApplication.h"

namespace PJobFileXMLFunctions
{
	QByteArray addParameterDefinition(const PJobFileParameterDefinition& def, QByteArray xmlFile);
	QByteArray removeParameterDefinition(QString parameterName, QByteArray xmlFile);
	QList<PJobFileParameterDefinition> readParameterDefinitions(QByteArray xmlFile);
	QByteArray writeParameterDefinitions(QList<PJobFileParameterDefinition>);

	QList<PJobFileParameter> readParameterCombination(QByteArray xmlFile);
	QByteArray writeParameterCombination(QList<PJobFileParameter>);
	
	QList<PJobResultFile> readResultDefinitions(QByteArray xmlFile);
	QByteArray writeResultDefinitions(QList<PJobResultFile>);

    QList<PJobFileApplication> readBinaries(QByteArray);
    QByteArray writeBinaries(QList<PJobFileApplication>);
};
