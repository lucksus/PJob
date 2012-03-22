#include "UserInputMediator.h"

UserInputMediator::UserInputMediator(void)
: m_handler(0)
{
}

UserInputMediator& UserInputMediator::getInstance(){
	static UserInputMediator u;
	return u;
}

QScriptValue UserInputMediator::userInputForScript(QScriptContext *context, QScriptEngine *engine){
	Q_ASSERT(m_handler);
	return m_handler->userInputForScript(context,engine);
}

void UserInputMediator::setUserInputHandler(UserInputHandler* handler){
	m_handler = handler;
}
