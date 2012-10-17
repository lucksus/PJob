#ifndef RESULTS_H
#define RESULTS_H
#include <QStringList>
#include <vector>
#include <QHash>

class Results : public QObject
{
Q_OBJECT
public:
    static Results& getInstance();
    void set_paramters(QHash<QString, QList<double> > m_known_parameter_values);
    void set_results(QStringList result_names);
    void set_value(QHash<QString,double> parameter_combination, QString result_name, double value);
    double value(QHash<QString,double> parameter_combination, QString result_name) const;

    QStringList parameter_names() const;
    QStringList result_names() const;
    unsigned int size() const;

    QHash<QString,double> parameter_combination_for_index(unsigned int);

public slots:
    void add_values(QHash<QString,double> parameters, QHash<QString,double> results);

signals:
    void updated();

private:
    Results();
    QHash<QString, double*> m_data;
    unsigned int m_data_size;
    QStringList m_parameters_sorted;
    QStringList m_results_sorted;
    QHash<QString, QList<double> > m_known_parameter_values;

    unsigned int index_for_parameter_combination(const QHash<QString, QList<double> >& known_parameter_values, const QHash<QString,double>& parameter_combination) const;
    void change_known_parameter_values(const QHash<QString, QList<double> >& new_known_parameter_values);
};

#endif // RESULTS_H

