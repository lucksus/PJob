#include "PJobFileXMLFunctions.h"
#include <iostream>

QByteArray PJobFileXMLFunctions::addParameterDefinition(const PJobFileParameterDefinition& def, QByteArray xmlFile){
	QDomDocument doc("parameterdefinitions");
	doc.setContent(xmlFile);
	QDomElement parameter = doc.createElement("parameter");
	doc.documentElement().appendChild(parameter);

	QDomElement name = doc.createElement("name");
	name.appendChild(doc.createTextNode(def.name()));
	parameter.appendChild(name);

	QDomElement defaultValue = doc.createElement("defaultValue");
	defaultValue.appendChild(doc.createTextNode(QString("%1").arg(def.defaultValue())));
	parameter.appendChild(defaultValue);

	if(def.hasUnit()){
		QDomElement unit = doc.createElement("unit");
		unit.appendChild(doc.createTextNode(def.unit()));
		parameter.appendChild(unit);
	}

	if(def.hasMinValue()){
		QDomElement min = doc.createElement("min");
		min.appendChild(doc.createTextNode(QString("%1").arg(def.minValue())));
		parameter.appendChild(min);
	}

	if(def.hasMaxValue()){
		QDomElement max = doc.createElement("max");
		max.appendChild(doc.createTextNode(QString("%1").arg(def.maxValue())));
		parameter.appendChild(max);
	}

	return doc.toString().toLocal8Bit();
}

QByteArray PJobFileXMLFunctions::removeParameterDefinition(QString parameterName, QByteArray xmlFile){
	QDomDocument doc("parameterdefinitions");
	doc.setContent(xmlFile);

	QDomElement parameteres = doc.documentElement();
	QDomNode parameter = parameteres.firstChild();
	while(!parameter.isNull()) {
		QDomElement param = parameter.toElement(); // try to convert the node to an element.
		if(param.isNull() || param.tagName() != "parameter") throw(QString("parameterdefinitions.xml is not valid!"));
		QDomNode node = param.firstChild();
		QString name,min,max;
		while(!node.isNull()){
			QDomElement elem = node.toElement();
                        if(elem.isNull() || (elem.tagName()!="name" && elem.tagName()!="defaultValue" && elem.tagName()!="min" && elem.tagName()!="max"))
                            throw(QString("parameterdefinitions.xml is not valid!"));
			if(elem.tagName()=="name") name = elem.text();
			node = node.nextSibling();
		}
		if(name == parameterName){
			parameteres.removeChild(parameter);
			break;
		}
		parameter = parameter.nextSibling();
	}

	return doc.toString().toLocal8Bit();
}

QList<PJobFileParameterDefinition> PJobFileXMLFunctions::readParameterDefinitions(QByteArray xmlFile){
	QDomDocument doc("parameterdefinitions");
	doc.setContent(xmlFile);

	QList<PJobFileParameterDefinition> result;
	QDomElement parameteres = doc.documentElement();
	QDomNode parameter = parameteres.firstChild();
	while(!parameter.isNull()) {
		QDomElement param = parameter.toElement(); // try to convert the node to an element.
		if(param.isNull() || param.tagName() != "parameter") throw(QString("parameterdefinitions.xml is not valid!"));
		QDomNode node = param.firstChild();
		QString name,min,max,unit;
		double defaultValue;
		while(!node.isNull()){
			QDomElement elem = node.toElement();
                        if(elem.isNull() || (elem.tagName()!="name" && elem.tagName()!="defaultValue" && elem.tagName()!="min" && elem.tagName()!="max" && elem.tagName()!="unit"))
                            throw(QString("parameterdefinitions.xml is not valid!"));
			if(elem.tagName()=="name") name = elem.text().trimmed();
			if(elem.tagName()=="defaultValue") defaultValue = elem.text().toDouble();
			if(elem.tagName()=="min") min = elem.text().trimmed();
			if(elem.tagName()=="max") max = elem.text().trimmed();
			if(elem.tagName()=="unit") unit = elem.text().trimmed();
			node = node.nextSibling();
		}
		PJobFileParameterDefinition p(name,defaultValue);
		if(min.length()) p.setMinValue(min.toDouble());
		if(max.length()) p.setMaxValue(max.toDouble());
		if(unit.length()) p.setUnit(unit);
		result.append(p);
		parameter = parameter.nextSibling();
	}

	return result;
}

QByteArray PJobFileXMLFunctions::writeParameterDefinitions(QList<PJobFileParameterDefinition> definitions){
	QDomDocument doc("parameterdefinitions");
	QDomElement root = doc.createElement("parameterdefinitions");
	doc.appendChild(root);

	PJobFileParameterDefinition d;
	foreach(d,definitions){
		QDomElement tag = doc.createElement("parameter");
		root.appendChild(tag);

		QDomElement name = doc.createElement("name");
		name.appendChild(doc.createTextNode(d.name()));
		tag.appendChild(name);

		QDomElement value = doc.createElement("defaultValue");
		value.appendChild(doc.createTextNode(QString("%1").arg(d.defaultValue())));
		tag.appendChild(value);

		if(d.hasMinValue()){
			QDomElement e = doc.createElement("min");
			e.appendChild(doc.createTextNode(QString("%1").arg(d.minValue())));
			tag.appendChild(e);
		}

		if(d.hasMaxValue()){
			QDomElement e = doc.createElement("max");
			e.appendChild(doc.createTextNode(QString("%1").arg(d.maxValue())));
			tag.appendChild(e);
		}

		if(d.hasUnit()){
			QDomElement e = doc.createElement("unit");
			e.appendChild(doc.createTextNode(QString("%1").arg(d.unit())));
			tag.appendChild(e);
		}
	}

	return doc.toString().toLocal8Bit();
}

QList<PJobFileParameter> PJobFileXMLFunctions::readParameterCombination(QByteArray xmlFile){
	QDomDocument doc("parametercombination");
	doc.setContent(xmlFile);

	QList<PJobFileParameter> result;
	QDomElement parameteres = doc.documentElement();
	QDomNode parameter = parameteres.firstChild();
	while(!parameter.isNull()) {
		QDomElement param = parameter.toElement(); // try to convert the node to an element.
		if(param.isNull() || param.tagName() != "parameter") throw(QString("parametercombination.xml is not valid!"));
		QDomNode node = param.firstChild();
		QString name,value,min,max,step;
		while(!node.isNull()){
			QDomElement elem = node.toElement();
                        if(elem.isNull() || (elem.tagName()!="name" && elem.tagName()!="value" && elem.tagName()!="variation")) throw(QString("parametercombination.xml is not valid!"));
			if(elem.tagName()=="name") name = elem.text();
			if(elem.tagName()=="value") value = elem.text();
			if(elem.tagName()=="variation"){
				QDomNode node2 = elem.firstChild();
				while(!node2.isNull()){
					QDomElement elem2 = node2.toElement();
                                        if(elem2.isNull() || (elem2.tagName()!="min" && elem2.tagName()!="max" && elem2.tagName()!="step")) throw(QString("parametercombination.xml is not valid!"));
					if(elem2.tagName()=="min") min = elem2.text();
					if(elem2.tagName()=="max") max = elem2.text();
					if(elem2.tagName()=="step") step = elem2.text();
					node2 = node2.nextSibling();
				}
			}
			
			node = node.nextSibling();
		}

		PJobFileParameter p;
		p.setName(name.trimmed());
		if(value.length())
			p.setValue(value.toDouble());
		else
			p.setVariation(min.toDouble(),max.toDouble(),step.toDouble());
		
		result.append(p);
		parameter = parameter.nextSibling();
	}

	return result;
}

QByteArray PJobFileXMLFunctions::writeParameterCombination(QList<PJobFileParameter> parameters){
	QDomDocument doc("parametercombination");
	QDomElement root = doc.createElement("parametercombination");
	doc.appendChild(root);

	PJobFileParameter p;
	foreach(p,parameters){
		QDomElement tag = doc.createElement("parameter");
		root.appendChild(tag);

		QDomElement name = doc.createElement("name");
		name.appendChild(doc.createTextNode(p.name()));
		tag.appendChild(name);

		if(!p.isVariation()){
			QDomElement value = doc.createElement("value");
			value.appendChild(doc.createTextNode(QString("%1").arg(p.value())));
			tag.appendChild(value);
		}else{
			QDomElement variation = doc.createElement("variation");
			tag.appendChild(variation);

			QDomElement min = doc.createElement("min");
			min.appendChild(doc.createTextNode(QString("%1").arg(p.minValue())));
			variation.appendChild(min);
			QDomElement max = doc.createElement("max");
			max.appendChild(doc.createTextNode(QString("%1").arg(p.maxValue())));
			variation.appendChild(max);
			QDomElement step = doc.createElement("step");
			step.appendChild(doc.createTextNode(QString("%1").arg(p.step())));
			variation.appendChild(step);
		}
	}

	return doc.toString().toLocal8Bit();
}

QList<PJobResultFile> PJobFileXMLFunctions::readResultDefinitions(QByteArray xmlFile){
	QDomDocument doc("resultdefinitions");
	doc.setContent(xmlFile);

	QString format;
	QList<PJobResultFile> result;
	QDomElement parameteres = doc.documentElement();
	QDomNode parameter = parameteres.firstChild();
	while(!parameter.isNull()) {
		QDomElement param = parameter.toElement(); // try to convert the node to an element.
		if(param.isNull() || param.tagName() != "resultFile") throw(QString("resultdefinitions.xml is not valid!"));
		QDomNode node = param.firstChild();
		PJobResultFile resultFile;
		while(!node.isNull()){
			QDomElement elem = node.toElement();
                        if(elem.isNull() || (elem.tagName()!="filename" && elem.tagName()!="format" && elem.tagName()!="result"))
                                throw(QString("resultdefinitions.xml is not valid!"));
			if(elem.tagName()=="filename") 
				resultFile.setFilename(elem.text().trimmed());
			if(elem.tagName()=="format")
				format = elem.text().trimmed();
			if(elem.tagName()=="result"){
				PJobResult r;
				QDomNode node2 = elem.firstChild();
				while(!node2.isNull()){
					QDomElement elem2 = node2.toElement();
                                        if(elem2.isNull() || (elem2.tagName()!="name" && elem2.tagName()!="unit")) throw(QString("resultdefinitions.xml is not valid!"));
					if(elem2.tagName()=="name") r.setName(elem2.text().trimmed());
					if(elem2.tagName()=="unit") r.setUnit(elem2.text().trimmed());
					node2 = node2.nextSibling();
				}
				resultFile.addResult(r);
			}

			node = node.nextSibling();
		}

                if(format.contains("CSV"))
                    resultFile.setType(PJobResultFile::CSV);
                else if(format.contains("SINGLE_VALUE"))
                    resultFile.setType(PJobResultFile::SINGLE_VALUE);
                else{
                    std::cout << format.toStdString() << std::endl;
                    std::cout.flush();
                    Q_ASSERT(false);
                }

		result.append(resultFile);
		parameter = parameter.nextSibling();
	}
	return result;
}

QByteArray PJobFileXMLFunctions::writeResultDefinitions(QList<PJobResultFile> resultFiles){
	QDomDocument doc("resultdefinitions");
	QDomElement root = doc.createElement("resultdefinitions");
	doc.appendChild(root);

	PJobResultFile f;
	foreach(f,resultFiles){
		QDomElement tag = doc.createElement("resultFile");
		root.appendChild(tag);

		QDomElement name = doc.createElement("filename");
		name.appendChild(doc.createTextNode(f.filename()));
		tag.appendChild(name);

		QDomElement format = doc.createElement("format");
                switch(f.type()){
                case PJobResultFile::SINGLE_VALUE:
                    format.appendChild(doc.createTextNode("SINGLE_VALUE"));
                    break;
                case PJobResultFile::CSV:
                    format.appendChild(doc.createTextNode("CSV"));
                    break;
                default:
                    Q_ASSERT(false);
                }
		tag.appendChild(format);
		
		PJobResult r;
		foreach(r,f.results()){
			QDomElement result = doc.createElement("result");
			tag.appendChild(result);
			
			QDomElement name = doc.createElement("name");
			name.appendChild(doc.createTextNode(r.name()));
			result.appendChild(name);

			QDomElement unit = doc.createElement("unit");
			unit.appendChild(doc.createTextNode(r.unit()));
			result.appendChild(unit);
		}
	}

	return doc.toString().toLocal8Bit();
}

QList<PJobFileBinary> PJobFileXMLFunctions::readBinaries(QByteArray xmlFile){
    QDomDocument doc("binaries");
    doc.setContent(xmlFile);

    QList<PJobFileBinary> result;
    QDomElement binaries = doc.documentElement();
    QDomNode binary_node = binaries.firstChild();
    while(!binary_node.isNull()) {
        QDomElement binary_element = binary_node.toElement(); // try to convert the node to an element.
        if(binary_element.isNull() || binary_element.tagName() != "binary") throw(QString("binaries.xml is not valid!"));
        QDomNode node = binary_element.firstChild();
        PJobFileBinary binary;
        while(!node.isNull()){
        QDomElement elem = node.toElement();
            if(elem.isNull() || (elem.tagName()!="name" && elem.tagName()!="program_name" && elem.tagName()!="program_version" && elem.tagName()!="platform" && elem.tagName()!="executable" && elem.tagName()!="parameter_pattern"))
                throw(QString("binaries.xml is not valid!"));
            if(elem.tagName()=="name") binary.name = elem.text().trimmed();
            if(elem.tagName()=="program_name") binary.program_name = elem.text().trimmed();
            if(elem.tagName()=="program_version") binary.program_version = elem.text().trimmed();
            if(elem.tagName()=="platform"){
                QString platform_string = elem.text().trimmed();
                if(platform_string=="Win32") binary.platform = PJobFileBinary::Win32;
                if(platform_string=="Win64") binary.platform = PJobFileBinary::Win64;
                if(platform_string=="MacOSX") binary.platform = PJobFileBinary::MacOSX;
                if(platform_string=="Linux") binary.platform = PJobFileBinary::Linux;
            }
            if(elem.tagName()=="executable") binary.executable = elem.text().trimmed();
            if(elem.tagName()=="parameter_pattern") binary.parameter_pattern = elem.text().trimmed();

        }
        binary_node = binary_node.nextSibling();
        result.append(binary);
    }
    return result;
}

QByteArray PJobFileXMLFunctions::writeBinaries(QList<PJobFileBinary> binaries){
    QDomDocument doc("binaries");
    QDomElement root = doc.createElement("binaries");
    doc.appendChild(root);

    PJobFileBinary binary;
    foreach(binary, binaries){
        QDomElement tag = doc.createElement("binary");
        root.appendChild(tag);

        QDomElement name = doc.createElement("name");
        name.appendChild(doc.createTextNode(binary.name));
        tag.appendChild(name);

        QDomElement program_name = doc.createElement("program_name");
        program_name.appendChild(doc.createTextNode(binary.program_name));
        tag.appendChild(program_name);

        QDomElement program_version = doc.createElement("program_version");
        program_version.appendChild(doc.createTextNode(binary.program_version));
        tag.appendChild(program_version);

        QDomElement platform = doc.createElement("platform");
                switch(binary.platform){
                case PJobFileBinary::Win32:
                    platform.appendChild(doc.createTextNode("Win32"));
                    break;
                case PJobFileBinary::Win64:
                    platform.appendChild(doc.createTextNode("Win64"));
                    break;
                case PJobFileBinary::MacOSX:
                    platform.appendChild(doc.createTextNode("MacOSX"));
                    break;
                case PJobFileBinary::Linux:
                    platform.appendChild(doc.createTextNode("Linux"));
                    break;
                default:
                    Q_ASSERT(false);
                }
        tag.appendChild(platform);

        QDomElement executable = doc.createElement("executable");
        executable.appendChild(doc.createTextNode(binary.executable));
        tag.appendChild(executable);

        QDomElement parameter_pattern = doc.createElement("parameter_pattern");
        parameter_pattern.appendChild(doc.createTextNode(binary.parameter_pattern));
        tag.appendChild(parameter_pattern);

    }

    return doc.toString().toLocal8Bit();
}
