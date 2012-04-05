#include "scriptengine.h"
#include "session.h"

ScriptEngine::ScriptEngine()
{
    m_engine.setGlobalObject(m_engine.newQObject(&Session::global_instance()));
}

void ScriptEngine::evaluate(const QString& code){
    try{
        Session::global_instance().output(m_engine.evaluate(code).toString());
    }catch(PJobFileError& e){
        Session::global_instance().output(e.msg());
    }
}
