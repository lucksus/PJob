#include "scriptengine.h"
#include "session.h"

ScriptEngine::ScriptEngine(Session* session)
    : m_session(session)
{
    m_engine.setGlobalObject(m_engine.newQObject(m_session));
    m_engine.globalObject().setProperty("hello", session->hello());
}

void ScriptEngine::evaluate(const QString& code){
    try{
        m_session->output(m_engine.evaluate(code).toString());
    }catch(PJobFileError& e){
        m_session->output(e.msg());
    }
}
