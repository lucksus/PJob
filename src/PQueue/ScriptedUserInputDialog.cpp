#include "ScriptedUserInputDialog.h"
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QFileDialog>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QSpinBox>
#include <QtGui/QButtonGroup>
#include <QtGui/QRadioButton>
#include <QtScript/QScriptEngine>
#include <limits>

FilenameWidget::FilenameWidget(QString filter, QString caption, QWidget* parent)
: QWidget(parent), m_filter(filter), m_caption(caption){
	setLayout(new QHBoxLayout);
	m_lineEdit = new QLineEdit(this);
	m_button = new QPushButton(this);
	m_button->setText("browse...");
	connect(m_button, SIGNAL(pressed()), this, SLOT(filenameDialog()));
	layout()->addWidget(m_lineEdit);
	layout()->addWidget(m_button);
}

QString FilenameWidget::filename(){
	return m_lineEdit->text();
}

void FilenameWidget::filenameDialog(){
	QString fileName = QFileDialog::getOpenFileName(this, m_caption, "", m_filter);
	m_lineEdit->setText(fileName);
}

QWidget* ScriptedUserInputDialog::createFilenameWidget(QScriptValue value){
	QString filter="",caption="";
	QScriptValue filterValue = value.property("filter");
	QScriptValue captionValue = value.property("caption");
	if(filterValue.isValid())
		filter = filterValue.toString();
	if(captionValue.isValid())
		caption = captionValue.toString();
	return new FilenameWidget(filter,caption,this);
}

QWidget* ScriptedUserInputDialog::createStringWidget(QScriptValue value){
	QLineEdit* edit = new QLineEdit(this);
	QScriptValue defaultValue = value.property("defaultValue");
	if(defaultValue.isValid()) edit->setText(defaultValue.toString());
	return edit;
}

QWidget* ScriptedUserInputDialog::createDoubleWidget(QScriptValue value){
	QDoubleSpinBox* spinBox = new QDoubleSpinBox(this);
	spinBox->setDecimals(5);
	QScriptValue minValue = value.property("min");
	QScriptValue maxValue = value.property("max");
	QScriptValue stepValue = value.property("step");
	QScriptValue defaultValue = value.property("defaultValue");
	spinBox->setMinimum(-std::numeric_limits<double>::max());
	spinBox->setMaximum(std::numeric_limits<double>::max());
	if(minValue.isValid()) spinBox->setMinimum(minValue.toNumber());
	if(maxValue.isValid()) spinBox->setMaximum(maxValue.toNumber());
	if(stepValue.isValid()) spinBox->setSingleStep(stepValue.toNumber());
	if(defaultValue.isValid()) spinBox->setValue(defaultValue.toNumber());
	return spinBox;
}

QWidget* ScriptedUserInputDialog::createIntegerWidget(QScriptValue value){
	QSpinBox* spinBox = new QSpinBox(this);
	QScriptValue minValue = value.property("min");
	QScriptValue maxValue = value.property("max");
	QScriptValue stepValue = value.property("step");
	QScriptValue defaultValue = value.property("defaultValue");
	spinBox->setMinimum(std::numeric_limits<int>::min());
	spinBox->setMaximum(std::numeric_limits<int>::max());
        if(minValue.isValid()) spinBox->setMinimum(static_cast<int>(minValue.toInteger()));
        if(maxValue.isValid()) spinBox->setMaximum(static_cast<int>(maxValue.toInteger()));
        if(stepValue.isValid()) spinBox->setSingleStep(static_cast<int>(stepValue.toInteger()));
        if(defaultValue.isValid()) spinBox->setValue(static_cast<int>(defaultValue.toInteger()));
	return spinBox;
}

QWidget* ScriptedUserInputDialog::createBooleanWidget(QScriptValue value, QLayout* layout){
	QButtonGroup* group = new QButtonGroup(this);
	QRadioButton* b1 = new QRadioButton(this);
	QRadioButton* b2 = new QRadioButton(this);
	group->addButton(b1);
	group->addButton(b2);
	QScriptValue falseName = value.property("falseName");
	QScriptValue trueName = value.property("trueName");
	if(falseName.isValid()) b1->setText(falseName.toString());
	else b1->setText("false");
	if(trueName.isValid()) b2->setText(trueName.toString());
	else b2->setText("true");
	layout->addWidget(b1);
	return b2;
}

ScriptedUserInputDialog::ScriptedUserInputDialog(QScriptContext *context, QScriptEngine *engine)
: m_engine(engine){
	setWindowTitle("Script requires input");
	setLayout(new QVBoxLayout);

	QScriptValue params = context->argument(0);
	QScriptValueIterator it(params);
	while (it.hasNext()) {
		it.next();
		QFrame* frame = new QFrame(this);
		frame->setLayout(new QHBoxLayout);
		QLabel* label = new QLabel(this);
		label->setText(it.name()+":");
		frame->layout()->addWidget(label);

		QWidget* widget;
		QString type = it.value().property("type").toString();
		if(type == "file"){
			widget = createFilenameWidget(it.value());
		}else if(type == "string"){
			widget = createStringWidget(it.value());
		}else if(type == "double"){
			widget = createDoubleWidget(it.value());
		}else if(type == "integer"){
			widget = createIntegerWidget(it.value());
		}else if(type == "boolean"){
			widget = createBooleanWidget(it.value(),frame->layout());
		}
		frame->layout()->addWidget(widget);
		m_widgets[it.name()] = widget;
		layout()->addWidget(frame);
	}

	m_button = new QPushButton;
	m_button->setText("Ok");
	connect(m_button,SIGNAL(pressed()),this,SLOT(accept()));
	layout()->addWidget(m_button);
	//resize(400, 500);
}

QScriptValue ScriptedUserInputDialog::getValues(){
	exec();
	QScriptValue parameters = m_engine->newObject();
	QString param;
	foreach(param,m_widgets.keys()){
		QWidget* widget = m_widgets[param];
		if(FilenameWidget* fw = qobject_cast<FilenameWidget*>(widget)){
			parameters.setProperty(param,QScriptValue(m_engine, fw->filename()));
		}else if(QLineEdit* le = qobject_cast<QLineEdit*>(widget)){
			parameters.setProperty(param,QScriptValue(m_engine, le->text()));
		}else if(QDoubleSpinBox* sb = qobject_cast<QDoubleSpinBox*>(widget)){
			parameters.setProperty(param,QScriptValue(m_engine, sb->value()));
		}else if(QSpinBox* sb = qobject_cast<QSpinBox*>(widget)){
			parameters.setProperty(param,QScriptValue(m_engine, sb->value()));
		}else if(QRadioButton* rb = qobject_cast<QRadioButton*>(widget)){
			parameters.setProperty(param,QScriptValue(m_engine, rb->isChecked()));
		}
	}
	return parameters;
}
