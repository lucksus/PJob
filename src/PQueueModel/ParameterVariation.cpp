#include "ParameterVariation.h"
#include <QStringList>

ParameterVariation::ParameterVariation()
    : m_combination_count_cache_set(false)
{
}


void ParameterVariation::add_parameter(QString name, double min, double max, double step){
    ParameterBoundaries b;
    b.min = min; b.max = max; b.step = step;
    m_bounds[name] = b;

}

QHash<QString, double> ParameterVariation::parameter_combination(unsigned int index) const{
    QStringList parameter_names = m_bounds.keys();
    parameter_names.sort();
    QStringListIterator it(parameter_names);
    it.toBack();
    QHash<QString, double> result;
    while(it.hasPrevious()){
        QString current_parameter_name = it.previous();
        unsigned int values_count = values_for_parameter(current_parameter_name);
        unsigned int index_for_current_parameter = index / values_count;
        index = index % values_count;
        result[current_parameter_name] = ith_value_for_parameter(current_parameter_name, index_for_current_parameter);
    }
    return result;
}

unsigned int ParameterVariation::combination_count() const{
    if(m_combination_count_cache_set) return m_combination_count_cache;
    unsigned int count=1;
    foreach(QString parameter_name, m_bounds.keys()){
        count *= values_for_parameter(parameter_name);
    }
    m_combination_count_cache = count;
    m_combination_count_cache_set = true;
    return count;
}

unsigned int ParameterVariation::parameter_count() const{
    return m_bounds.size();
}

unsigned int ParameterVariation::values_for_parameter(QString parameter_name) const{
    if(!m_bounds.contains(parameter_name)) throw QString("ParameterVariation: No such parameter \"%\"!").arg(parameter_name);
    if(m_values_for_parameters_cache.contains(parameter_name)) return m_values_for_parameters_cache.find(parameter_name).operator *();
    unsigned int count;
    ParameterBoundaries b = m_bounds.find(parameter_name).operator *();
    for(count=1; b.min + count*b.step  <  b.step;count++);
    m_values_for_parameters_cache[parameter_name] = count;
    return count;
}

int ParameterVariation::parameter_index(QString parameter_name) const{
    QStringList names = m_bounds.keys();
    names.sort();
    return names.indexOf(parameter_name);
}


double ParameterVariation::ith_value_for_parameter(QString parameter_name, unsigned int index) const{
    if(!m_bounds.contains(parameter_name)) throw QString("ParameterVariation: No such parameter \"%\"!").arg(parameter_name);
    ParameterBoundaries b = m_bounds.find(parameter_name).operator *();
    return b.min + index*b.step;
}
