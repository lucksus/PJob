#include "MainWindowWithProgressPopups.h"
#include <QtGui/QDialog>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <assert.h>
#include <QtCore/QCoreApplication>

MainWindowWithProgressPopups::MainWindowWithProgressPopups(void)
{
}

MainWindowWithProgressPopups::~MainWindowWithProgressPopups(void)
{
}

void MainWindowWithProgressPopups::add_calculator_object(QObject* calculator){
	connect(calculator, SIGNAL(progress(QString, unsigned int)), this, SLOT(show_progress(QString, unsigned int)));
	connect(&m_progress_abort_mapper, SIGNAL(mapped(const QString &)), calculator, SLOT(abort_progress(const QString &)));
}

void MainWindowWithProgressPopups::remove_calculator_object(QObject* calculator){
	disconnect(calculator, SIGNAL(progress(QString, unsigned int)), this, SLOT(show_progress(QString, unsigned int)));
	disconnect(&m_progress_abort_mapper, SIGNAL(mapped(const QString &)), calculator, SLOT(abort_progress(const QString &)));
}

void MainWindowWithProgressPopups::show_progress(QString message, unsigned int percent){
	if(!m_progress_widgets.contains(message)){
		QDialog* widget = new QDialog(this,Qt::Sheet);
		widget->setModal(true);
		//widget->setCaption("Progress");
		QVBoxLayout* layout = new QVBoxLayout;
		widget->setLayout(layout);
		layout->addWidget(new QLabel(message,widget));
		QProgressBar* bar = new QProgressBar(widget);
		bar->setMinimum(0);
		bar->setMaximum(100);
		bar->setObjectName("progress_bar");
		layout->addWidget(bar);
		QPushButton* button = new QPushButton("Abort", this);
		connect(button, SIGNAL(clicked()), &m_progress_abort_mapper, SLOT(map()));
		m_progress_abort_mapper.setMapping(button, message);
		layout->addWidget(button);
		m_progress_widgets[message] = widget;
	}

	QWidget* widget = m_progress_widgets[message];
	assert(widget);
	widget->show();
	if(percent >= 100){
		m_progress_widgets.remove(message);
		widget->hide();
		delete widget;
		return;
	}

	//QObject* child = widget->findChild("progress_bar");
	QProgressBar* progress_bar = widget->findChild<QProgressBar*>("progress_bar");//qobject_cast<QProgressBar*>(child);
	assert(progress_bar);
	progress_bar->setValue(percent);

	QCoreApplication::processEvents();
}

