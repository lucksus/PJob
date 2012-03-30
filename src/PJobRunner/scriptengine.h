#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H
#include <QtScript/QScriptEngine>

class ScriptEngine
{
public:
    ScriptEngine();
    void evaluate(const QString& code);

private:
    QScriptEngine m_engine;
};

#endif // SCRIPTENGINE_H
