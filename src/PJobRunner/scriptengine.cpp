#include "scriptengine.h"
#include "session.h"

ScriptEngine::ScriptEngine()
{
    m_engine.setGlobalObject(m_engine.newQObject(&Session::instance()));
}

void ScriptEngine::evaluate(const QString& code){
    try{
        Session::instance().output(m_engine.evaluate(code).toString());
    }catch(PJobFileError& e){
        Session::instance().output(e.msg());
    }
}
