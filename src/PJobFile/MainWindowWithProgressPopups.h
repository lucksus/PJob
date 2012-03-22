#pragma once
#include <QtGui/QMainWindow>
#include <QtCore/QMap>
#include <QtCore/QSignalMapper>

class MainWindowWithProgressPopups : public QMainWindow
{
Q_OBJECT
public:
	MainWindowWithProgressPopups(void);
	~MainWindowWithProgressPopups(void);

protected:
	void add_calculator_object(QObject*);
	void remove_calculator_object(QObject*);

private slots:
	void show_progress(QString message, unsigned int percent);

private:
	QMap<QString,QWidget*> m_progress_widgets;
	QSignalMapper m_progress_abort_mapper;
};
