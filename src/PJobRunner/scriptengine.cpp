#include "scriptengine.h"
#include "controller.h"

ScriptEngine::ScriptEngine()
{
    m_engine.setGlobalObject(m_engine.newQObject(&Controller::instance()));
}

void ScriptEngine::evaluate(const QString& code){
    try{
        Controller::instance().output(m_engine.evaluate(code).toString());
    }catch(PJobFileError& e){
        Controller::instance().output(e.msg());
    }
}
