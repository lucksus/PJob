#pragma once

#include <QtGui/QDialog>
#include <QtGui/QPushButton>
#include <QtGui/QLineEdit>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptValueIterator>
#include <QtScript/QScriptContext>
#include <QtCore/QHash>

class FilenameWidget : public QWidget{
Q_OBJECT
public:
	FilenameWidget(QString filter, QString caption, QWidget* parent=0);
	QString filename();
private slots:
	void filenameDialog();
private:
	QPushButton* m_button;
	QLineEdit* m_lineEdit;
	QString m_filter, m_caption;
};



class ScriptedUserInputDialog : public QDialog{
Q_OBJECT
public:
	ScriptedUserInputDialog(QScriptContext *context, QScriptEngine *engine);
	QScriptValue getValues();
private:
	QHash<QString, QWidget*> m_widgets;
	QScriptEngine* m_engine;
	QPushButton* m_button;

	QWidget* createFilenameWidget(QScriptValue);
	QWidget* createStringWidget(QScriptValue);
	QWidget* createDoubleWidget(QScriptValue);
	QWidget* createIntegerWidget(QScriptValue);
	QWidget* createBooleanWidget(QScriptValue, QLayout*);

};
