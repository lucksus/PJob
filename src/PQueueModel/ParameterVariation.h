#ifndef PARAMETERVARIATION_H
#define PARAMETERVARIATION_H
#include <QHash>

class ParameterVariation
{
public:
    ParameterVariation();
    void add_parameter(QString name, double min, double max, double step);
    QHash<QString, double> parameter_combination(unsigned int index) const;
    inline unsigned int combination_count() const;
    inline unsigned int parameter_count() const;


private:
        struct ParameterBoundaries{
            double min, max, step;
        };

    QHash<QString, ParameterBoundaries> m_bounds;
    inline unsigned int values_for_parameter(QString) const;
    mutable QHash<QString, unsigned int> m_values_for_parameters_cache;
    inline int parameter_index(QString parameter_name) const;
    inline double ith_value_for_parameter(QString parameter_name, unsigned int index) const;
    mutable bool m_combination_count_cache_set;
    mutable unsigned int m_combination_count_cache;

};

#endif // PARAMETERVARIATION_H
