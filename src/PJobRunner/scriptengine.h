#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H
#include <QtScript/QScriptEngine>

class Session;
class ScriptEngine
{
public:
    ScriptEngine(Session* session);
    void evaluate(const QString& code);

private:
    QScriptEngine m_engine;
    Session* m_session;
};

#endif // SCRIPTENGINE_H
