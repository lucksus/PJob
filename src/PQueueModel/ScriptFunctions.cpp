#include "ScriptFunctions.h"
#include "PQueueController.h"
#include "Job.h"
#include "InterpolationFunction.h"
#include "LatinHypercubeDesign.h"
#include "Scripter.h"
#include "UserInputMediator.h"
#include <QtCore/QFileInfo>
#include "PJobFileRepository.h"

QScriptValue getSetScriptProgress(QScriptContext *ctx, QScriptEngine *eng){
	QScriptValue result;
	result = QScriptValue(eng,0);
	if (ctx->argumentCount() == 1) {
                int progress = static_cast<int>(ctx->argument(0).toInteger());
		result = QScriptValue(eng,progress);
		Scripter::getInstance().setProgress(eng,progress);
	}
	return result;
}

QScriptValue PhotossJobConstructor(QScriptContext *context, QScriptEngine *engine){
        QStringHash arg1 = engine->fromScriptValue<QStringHash>(context->argument(0));
        Job* pj = new Job(arg1);
	return engine->newQObject(pj);
}

QScriptValue InterpolationFunctionConstructor(QScriptContext *context, QScriptEngine *engine){
	QString arg1 = qscriptvalue_cast<QString> (context->argument(0)); 
	QString arg2 = qscriptvalue_cast<QString> (context->argument(1)); 
	//qscriptvalue_cast< QStringHash > (context->argument(1)); 
	InterpolationFunction* i = new InterpolationFunction(arg1,arg2); 
	return engine->newQObject(i, QScriptEngine::ScriptOwnership);
}

QScriptValue toScriptValueQStringHash(QScriptEngine *engine, const QStringHash &hash)
{
	QScriptValue obj = engine->newObject();
	QString s;
	foreach(s,hash.keys())
		obj.setProperty(s, QScriptValue(engine, hash[s]));
	return obj;
}

void fromScriptValueQStringHash(const QScriptValue &obj, QStringHash &s)
{
	QScriptValueIterator it(obj);
	while (it.hasNext()) {
		it.next();
		s[it.name()] = it.value().toString();
	}
}

QScriptValue toScriptValueQDoubleHash(QScriptEngine *engine, const QDoubleHash &hash){
	QScriptValue obj = engine->newObject();
	QString s;
	foreach(s,hash.keys())
		obj.setProperty(s, QScriptValue(engine, hash[s]));
	return obj;
}

void fromScriptValueQDoubleHash(const QScriptValue &obj, QDoubleHash &s){
	QScriptValueIterator it(obj);
	while (it.hasNext()) {
		it.next();
		s[it.name()] = it.value().toNumber();
	}
}

QScriptValue createLHDPoints(QScriptContext *ctx, QScriptEngine *eng)
{
	QScriptValue scriptRegion = ctx->argument(0);
        int sampleCount = static_cast<int>(ctx->argument(1).toInteger());

	QHash< QString, QPair<double,double> > region;
	QScriptValueIterator it(scriptRegion);
	while (it.hasNext()) {
		it.next();
		QString name = it.name();
		double min = it.value().property("min").toNumber();
		double max = it.value().property("max").toNumber();
		region[name] = QPair<double,double>(min,max);

	}

	LatinHypercubeDesign lhd(region, sampleCount);
	QVector< QHash<QString, double> > points = lhd.getDesignPoints();

	QScriptValue array = eng->evaluate("new Array()");
	QHash<QString, double> point;
	int i=0;
	foreach(point, points){
		QScriptValue scriptPoint = eng->newObject();
		QString parameter;
		foreach(parameter,point.keys()){
			scriptPoint.setProperty(parameter, QScriptValue(eng, point[parameter]));
		}
		array.setProperty(i,scriptPoint);
		++i;
	}

	return array;
}


QScriptValue print(QScriptContext *context, QScriptEngine *engine){
	QString result;
	for (int i = 0; i < context->argumentCount(); ++i) {
		if (i > 0)
			result.append(" ");
		result.append(context->argument(i).toString());
	}

	Scripter::getInstance().addOutputStringForEngine(engine,result);

	return engine->undefinedValue();
}

QScriptValue userInput(QScriptContext *context, QScriptEngine *engine){
	return UserInputMediator::getInstance().userInputForScript(context,engine);
}

QScriptValue readParametersFromPJOBFile(QScriptContext *context, QScriptEngine *engine){
	QString pjobFile = context->argument(0).toString();
	QList<PJobFileParameterDefinition> params ;
	try{
		PJobFile* pjob = PJobFileRepository::getInstance().forFile(pjobFile);
		params = pjob->parameterDefinitions();
	}catch(PJobFileError e){
		context->throwError(e.msg());
		return QScriptValue();
	}
	PJobFileRepository::getInstance().decreaseCounter(pjobFile);
	QScriptValue result = engine->newObject();
	PJobFileParameterDefinition param;
	foreach(param,params){
		QScriptValue paramEngine = engine->newObject();
		paramEngine.setProperty("defaultValue",QScriptValue(engine,param.defaultValue()));
		if(param.hasMinValue()) paramEngine.setProperty("minValue",QScriptValue(engine,param.minValue()));
		if(param.hasMaxValue()) paramEngine.setProperty("maxValue",QScriptValue(engine,param.maxValue()));
		result.setProperty(param.name(),paramEngine);
	}
	return result;
}

QScriptValue isExistingFile(QScriptContext *context, QScriptEngine *engine){
	QString phoFile = context->argument(0).toString();
	return QScriptValue(engine, QFileInfo(phoFile).exists() && QFileInfo(phoFile).isFile());
}

QScriptValue exportResults(QScriptContext *context, QScriptEngine *){
	QString outputfile = context->argument(0).toString();
	QString jobFile = context->argument(1).toString();
	QStringList pjobFiles(jobFile);
	PQueueController::getInstace().getResults().exportToCSV(outputfile,pjobFiles);
	return QScriptValue();
}

void addFunctionsToEngine(QScriptEngine* engine){
	engine->globalObject().setProperty("proxy",engine->newObject());

	QScriptValue controller = engine->newQObject(&PQueueController::getInstace());
	engine->globalObject().setProperty("PQueue", controller);

	QScriptValue scriptObject = engine->newObject();
	scriptObject.setProperty("progress",engine->newFunction(getSetScriptProgress), QScriptValue::PropertyGetter|QScriptValue::PropertySetter);
	engine->globalObject().setProperty("Script",scriptObject);

	QScriptValue photossJobCtor = engine->newFunction(PhotossJobConstructor);
	QScriptValue photossJobMetaObject = engine->newQMetaObject(&QObject::staticMetaObject, photossJobCtor);
	engine->globalObject().setProperty("PhotossJob", photossJobMetaObject);

	QScriptValue interpolantCtor = engine->newFunction(InterpolationFunctionConstructor);
	QScriptValue interpolantMetaObject = engine->newQMetaObject(&QObject::staticMetaObject, interpolantCtor);
	engine->globalObject().setProperty("InterpolationFunction", interpolantMetaObject);

	QScriptValue createLHDFunction = engine->newFunction(createLHDPoints);
	QScriptValue createLHDMetaObject = engine->newQMetaObject(&QObject::staticMetaObject, createLHDFunction);
	engine->globalObject().setProperty("createLHDPoints", createLHDMetaObject);

	QScriptValue printFunction = engine->newFunction(print);
	QScriptValue printFunctionMetaObject = engine->newQMetaObject(&QObject::staticMetaObject, printFunction);
	engine->globalObject().setProperty("print", printFunctionMetaObject);

	QScriptValue userInputFunction = engine->newFunction(userInput);
	QScriptValue userInputFunctionMetaObject = engine->newQMetaObject(&QObject::staticMetaObject, userInputFunction);
	engine->globalObject().setProperty("userInput", userInputFunction);

	QScriptValue readVariablesFromPJOBFileFunction = engine->newFunction(readParametersFromPJOBFile);
	QScriptValue readVariablesFromPJOBFileFunctionMetaObject = engine->newQMetaObject(&QObject::staticMetaObject, readVariablesFromPJOBFileFunction);
	engine->globalObject().setProperty("readParametersFromPJOBFile", readVariablesFromPJOBFileFunctionMetaObject);

	QScriptValue isExistingFileFunction = engine->newFunction(isExistingFile);
	QScriptValue isExistingFileFunctionMetaObject = engine->newQMetaObject(&QObject::staticMetaObject, isExistingFileFunction);
	engine->globalObject().setProperty("isExistingFile", isExistingFileFunctionMetaObject);

	QScriptValue exportResultsFunction = engine->newFunction(exportResults);
	QScriptValue exportResultsFunctionMetaObject = engine->newQMetaObject(&QObject::staticMetaObject, exportResultsFunction);
	engine->globalObject().setProperty("exportResults", exportResultsFunctionMetaObject);

	qScriptRegisterMetaType(engine, toScriptValueQStringHash, fromScriptValueQStringHash);
	qScriptRegisterMetaType(engine, toScriptValueQDoubleHash, fromScriptValueQDoubleHash);
}
