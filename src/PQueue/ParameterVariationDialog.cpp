#include "ParameterVariationDialog.h"
#include "ParameterVariation.h"
#include <QHBoxLayout>
#include <limits>
#include <assert.h>

ParameterWidget::ParameterWidget(ParameterVariationDialog* parent, QString parameter_name)
    : QWidget(parent)
{
    ParameterVariation::ParameterBoundaries bounds = parent->parameter_variation()->boundaries_for_parameter(parameter_name);
    QHBoxLayout* l = new QHBoxLayout;
    setLayout(l);
    m_parameter_name.setText(parameter_name);
    m_min.setPrefix("min:");
    m_min.setMinimum(-std::numeric_limits<double>::max());
    m_min.setMaximum(std::numeric_limits<double>::max());
    m_min.setValue(bounds.min);
    m_max.setPrefix("max:");
    m_max.setValue(bounds.max);
    m_max.setMinimum(-std::numeric_limits<double>::max());
    m_max.setMaximum(std::numeric_limits<double>::max());
    m_step.setPrefix("step:");
    m_step.setValue(bounds.step);
    m_step.setMinimum(-std::numeric_limits<double>::max());
    m_step.setMaximum(std::numeric_limits<double>::max());
    m_combination_count.setText(QString("%1").arg(static_cast<int>(1+(bounds.max - bounds.min)/bounds.step)));

    l->addWidget(&m_parameter_name);
    l->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Expanding));
    l->addWidget(&m_combination_count);
    l->addSpacing(10);
    l->addWidget(&m_min);
    l->addWidget(&m_max);
    l->addWidget(&m_step);


    connect(&m_min, SIGNAL(valueChanged(double)), this, SLOT(_values_changed()));
    connect(&m_max, SIGNAL(valueChanged(double)), this, SLOT(_values_changed()));
    connect(&m_step, SIGNAL(valueChanged(double)), this, SLOT(_values_changed()));
}

void ParameterWidget::_values_changed(){
    ParameterVariationDialog* dialog = dynamic_cast<ParameterVariationDialog*>(parent());
    assert(dialog);
    if(!dialog) return;
    dialog->parameter_variation()->set_boundaries_for_parameter(m_parameter_name.text(), get_boundaries());
    QString values_count = QString("%1").arg(dialog->parameter_variation()->values_for_parameter(m_parameter_name.text()));
    m_combination_count.setText(values_count);
    emit values_changed();
}

ParameterVariation::ParameterBoundaries ParameterWidget::get_boundaries() const{
    ParameterVariation::ParameterBoundaries boundaries;
    boundaries.min  = m_min.value();
    boundaries.max  = m_max.value();
    boundaries.step = m_step.value();
    return boundaries;
}

ParameterVariationDialog::ParameterVariationDialog(ParameterVariation pv, QWidget *parent) :
    QDialog(parent), m_pv(pv)
{
    QVBoxLayout* lv = new QVBoxLayout;
    setLayout(lv);
    QStringList parameter_names = pv.parameter_names();
    parameter_names.sort();
    foreach(QString parameter_name, parameter_names){
        ParameterWidget* widget = new ParameterWidget(this, parameter_name);
        layout()->addWidget(widget);
        m_parameter_widgets[parameter_name] = widget;
        connect(widget, SIGNAL(values_changed()), this, SLOT(values_changed()));
    }
    QHBoxLayout *l1 = new QHBoxLayout;
    l1->addWidget(new QLabel("Total combination count: "));
    l1->addWidget(&m_combination_count);
    m_combination_count.setText(QString("%1").arg(m_pv.combination_count()));
    lv->addLayout(l1);
    QHBoxLayout *l = new QHBoxLayout;
    l->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Expanding));
    l->addWidget(&m_cancel_button);
    l->addWidget(&m_ok_button);
    lv->addLayout(l);

    connect(&m_cancel_button, SIGNAL(clicked()), this, SLOT(close()));
    connect(&m_ok_button, SIGNAL(clicked()), this, SLOT(ok()));
    m_ok_button.setText("Run variation!");
    m_cancel_button.setText("Cancel");
}

ParameterVariation* ParameterVariationDialog::parameter_variation(){
    return &m_pv;
}

void ParameterVariationDialog::ok(){
    accept();
}

void ParameterVariationDialog::values_changed(){
    m_combination_count.setText(QString("%1").arg(m_pv.combination_count()));
}
