#pragma once
#include <QtScript/QScriptContext>
#include <QtScript/QScriptEngine>

class UserInputHandler{
public:
	virtual QScriptValue userInputForScript(QScriptContext *context, QScriptEngine *engine) = 0;
};

class UserInputMediator
{
public:
	static UserInputMediator& getInstance();
	QScriptValue userInputForScript(QScriptContext *context, QScriptEngine *engine);
	void setUserInputHandler(UserInputHandler*);

private:
	UserInputMediator(void);
	UserInputHandler* m_handler;
};
