#pragma  once
#include <QtScript/QScriptValue>
#include <QtScript/QScriptValueIterator>
#include <QtScript/QScriptContext>
#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QMetaType>

typedef QHash<QString,QString> QStringHash;
Q_DECLARE_METATYPE( QStringHash );
typedef QHash<QString,double> QDoubleHash;
Q_DECLARE_METATYPE( QDoubleHash );

QScriptValue getSetScriptProgress(QScriptContext *ctx, QScriptEngine *eng);
QScriptValue PhotossJobConstructor(QScriptContext *context, QScriptEngine *engine);
QScriptValue InterpolationFunctionConstructor(QScriptContext *context, QScriptEngine *engine);
QScriptValue toScriptValueQStringHash(QScriptEngine *engine, const QStringHash &hash);
void fromScriptValueQStringHash(const QScriptValue &obj, QStringHash &s);
QScriptValue toScriptValueQDoubleHash(QScriptEngine *engine, const QDoubleHash &hash);
void fromScriptValueQDoubleHash(const QScriptValue &obj, QDoubleHash &s);
QScriptValue createLHDPoints(QScriptContext *ctx, QScriptEngine *eng);
QScriptValue print(QScriptContext *context, QScriptEngine *engine);
QScriptValue setProgress(QScriptContext *context, QScriptEngine *engine);
QScriptValue readGlobalVariablesFromPHOFile(QScriptContext *context, QScriptEngine *engine);
QScriptValue readParametersFromPJOBFile(QScriptContext *context, QScriptEngine *engine);
QScriptValue isExistingFile(QScriptContext *context, QScriptEngine *engine);
QScriptValue exportResults(QScriptContext *context, QScriptEngine *engine);
void addFunctionsToEngine(QScriptEngine* engine);
