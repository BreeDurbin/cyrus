#pragma once

#include "../Creature/Creature.h"
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QWidget>

class CreatureEditor : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(Creature creature READ value WRITE setValue USER true)

public:
    CreatureEditor(QWidget *parent = nullptr);
    void setValue(const Creature &creature);
    Creature value() const;

private:
    QString m_name;
    QComboBox *m_role;
    QSpinBox *m_initiative;
};

