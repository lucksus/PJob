#ifndef PARAMETERVARIATIONDIALOG_H
#define PARAMETERVARIATIONDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QDoubleSpinBox>
#include "ParameterVariation.h"
#include <QPushButton>

class ParameterVariationDialog;

class ParameterWidget : public QWidget
{
Q_OBJECT
public:
    ParameterWidget(ParameterVariationDialog*, QString parameter_name);
    ParameterVariation::ParameterBoundaries get_boundaries() const;

signals:
    void values_changed();

private:
    QLabel m_parameter_name;
    QLabel m_combination_count;
    QDoubleSpinBox m_min, m_max, m_step;

private slots:
    void _values_changed();
};


class ParameterVariationDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ParameterVariationDialog(ParameterVariation, QWidget *parent = 0);
    ParameterVariation* parameter_variation();
    
signals:
    
public slots:
private:
    ParameterVariation m_pv;
    QHash<QString, ParameterWidget*> m_parameter_widgets;
    QPushButton m_ok_button, m_cancel_button;
    QLabel m_combination_count;
private slots:
    void ok();
    void values_changed();
};

#endif // PARAMETERVARIATIONDIALOG_H
