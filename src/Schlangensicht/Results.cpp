#include "Results.h"
#include <assert.h>

Results::Results()
    : m_data_size(0)
{
}

Results& Results::getInstance(){
    static Results r;
    return r;
}

void Results::set_paramters(QHash<QString, QList<double> > parameter_values){
    change_known_parameter_values(parameter_values);
}

void Results::set_results(QStringList result_names){
    m_results_sorted = result_names;
    m_results_sorted.sort();
    foreach(QString result, m_data.keys()){
        if(!m_results_sorted.contains(result)){
            delete m_data[result];
            m_data.remove(result);
        }
    }

    foreach(QString result, m_results_sorted){
        if(!m_data.keys().contains(result)){
            m_data[result] = new double[m_data_size];
            memset(m_data[result], 0, m_data_size*sizeof(double));
        }
    }
}

void Results::set_value(QHash<QString,double> parameter_combination, QString result_name, double value){
    if(!m_results_sorted.contains(result_name)) set_results(m_results_sorted << result_name);
    bool new_parameter_value = false;
    QHash<QString, QList<double> > new_known_parameter_values = m_known_parameter_values;
    foreach(QString parameter, parameter_combination.keys()){
        double parameter_value = parameter_combination[parameter];
        if(!m_known_parameter_values.keys().contains(parameter) || !m_known_parameter_values[parameter].contains(parameter_value)){
            new_known_parameter_values[parameter].append(parameter_value);
            qSort(new_known_parameter_values[parameter].begin(), new_known_parameter_values[parameter].end());
            new_parameter_value = true;
        }
    }

    if(new_parameter_value){
        change_known_parameter_values(new_known_parameter_values);
    }

    unsigned int index = index_for_parameter_combination(m_known_parameter_values, parameter_combination);
    assert(index < m_data_size);
    m_data[result_name][index] = value;
    emit updated();
}


double Results::value(QHash<QString,double> parameter_combination, QString result_name) const{
    assert(m_results_sorted.contains(result_name));
    unsigned int index = index_for_parameter_combination(m_known_parameter_values, parameter_combination);
    assert(index < m_data_size);
    return m_data[result_name][index];
}

void Results::add_values(QHash<QString,double> parameters, QHash<QString,double> results){
    foreach(QString result_name, results.keys()){
        set_value(parameters, result_name, results[result_name]);
    }
}


unsigned int Results::index_for_parameter_combination(const QHash<QString, QList<double> >& known_parameter_values, const QHash<QString,double>& parameter_combination) const{
    unsigned int index = 0;
    unsigned int multiplier = 1;
    foreach(QString parameter, m_parameters_sorted){
        unsigned int index_of_parameter_value = known_parameter_values[parameter].indexOf(parameter_combination[parameter]);
        index += index_of_parameter_value * multiplier;
        multiplier *= known_parameter_values[parameter].size();
    }
    return index;
}

class CombinationIterator{
public:
    CombinationIterator(const QHash<QString, QList<double> >& parameter_values) : m_parameter_values(parameter_values){
        foreach(QString parameter, parameter_values.keys()){
            m_indices[parameter] = 0;
        }
    }

    bool has_next(){
        if(m_indices.isEmpty()) return false;
        foreach(QString parameter, m_indices.keys()){
            if(static_cast<int>(m_indices[parameter]) >= m_parameter_values[parameter].size()) return false;
        }
        return true;
    }

    QHash<QString,double> next(){
        QHash<QString,double> combination;
        QStringList parameter_names = m_indices.keys();
        parameter_names.sort();

        foreach(QString parameter, parameter_names){
            combination[parameter] = m_parameter_values[parameter].at(m_indices[parameter]);
        }

        QString last_parameter = parameter_names.back();
        if(has_next()){
            int i = 0;
            bool done=false;
            while(!done){
                QString parameter = parameter_names.at(i);
                m_indices[parameter]++;
                done = true;
                i++;
                if(static_cast<int>(m_indices[parameter]) >= m_parameter_values[parameter].size() && parameter != last_parameter){
                    m_indices[parameter] = 0;
                    done = false;
                }

            }
        }

        return combination;
    }

private:
    QHash<QString, QList<double> > m_parameter_values;
    QHash<QString, int > m_indices;
};

void Results::change_known_parameter_values(const QHash<QString, QList<double> >& new_known_parameter_values){
    unsigned int new_size=1;
    foreach(QString parameter, new_known_parameter_values.keys()){
        new_size *= new_known_parameter_values[parameter].size();
    }

    foreach(QString result_name, m_results_sorted){

        double* new_data = new double[new_size];
        memset(new_data, 0, sizeof(double)*new_size);

        double* old_data = m_data[result_name];

        if(old_data){
            CombinationIterator it(m_known_parameter_values);
            while(it.has_next()){
                QHash<QString,double> combination = it.next();
                new_data[index_for_parameter_combination(new_known_parameter_values, combination)] = old_data[index_for_parameter_combination(m_known_parameter_values, combination)];
            }

            delete m_data[result_name];
        }
        m_data[result_name] = new_data;
    }

    m_data_size = new_size;
    m_known_parameter_values = new_known_parameter_values;
    m_parameters_sorted = m_known_parameter_values.keys();
    m_parameters_sorted.sort();
}

QStringList Results::parameter_names() const{
    return m_parameters_sorted;
}

QStringList Results::result_names() const{
    return m_results_sorted;
}

unsigned int Results::size() const{
    return m_data_size;
}

QHash<QString,double> Results::parameter_combination_for_index(unsigned int index){
    QStringListIterator it(m_parameters_sorted);
    it.toBack();
    QHash<QString, double> result;
    while(it.hasPrevious()){
        QString current_parameter_name = it.previous();
        unsigned int values_count = m_known_parameter_values[current_parameter_name].size();
        unsigned int index_for_current_parameter = index % values_count;
        index = index / values_count;
        result[current_parameter_name] = m_known_parameter_values[current_parameter_name][index_for_current_parameter];
    }
    return result;
}
