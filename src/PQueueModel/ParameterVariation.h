#ifndef PARAMETERVARIATION_H
#define PARAMETERVARIATION_H
#include <QHash>

class ParameterVariation
{
public:
    ParameterVariation();
    void add_parameter(QString name, double min, double max, double step);

    struct ParameterBoundaries{
        double min, max, step;
    };

    unsigned int combination_count() const;
    unsigned int parameter_count() const;
    unsigned int values_for_parameter(QString) const;

    bool index_valid() const;
    QHash<QString, double> parameter_combination() const;
    void next();
    void reset();
    ParameterBoundaries boundaries_for_parameter(QString name) const;
    void set_boundaries_for_parameter(QString name, ParameterBoundaries b);
    QStringList parameter_names() const;


private:


    QHash<QString, ParameterBoundaries> m_bounds;
    mutable QHash<QString, unsigned int> m_values_for_parameters_cache;
    inline int parameter_index(QString parameter_name) const;
    inline double ith_value_for_parameter(QString parameter_name, unsigned int index) const;
    mutable bool m_combination_count_cache_set;
    mutable unsigned int m_combination_count_cache;

    QHash<QString, double> parameter_combination(unsigned int index) const;

    unsigned int m_current_index;

};

#endif // PARAMETERVARIATION_H
