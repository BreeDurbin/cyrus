

#include "CreatureEditor.h"
#include "../Creature/Creature.h"
#include <QHBoxLayout>
#include <QTimer>

CreatureEditor::CreatureEditor(QWidget *parent)
    : QWidget(parent)
{
    
    auto layout = new QHBoxLayout(this);
    m_role = new QComboBox;
    m_role->addItems({"PC", "Monster", "NPC"});
    layout->addWidget(m_role);

    m_initiative = new QSpinBox;
    m_initiative->setMaximum(INT_MAX);
    layout->addWidget(m_initiative, 1);

    layout->setContentsMargins(10, 10, 75, 10);
    setAutoFillBackground(true);
}

void CreatureEditor::setValue(const Creature &creature)
{
    // This is indeed very brittle, but after all it is just an example.
    m_name = creature.name();
    m_role->setCurrentIndex(creature.role());
    m_initiative->setValue(creature.initiative());
}

Creature CreatureEditor::value() const
{
    return Creature(m_name, (Creature::Role)m_role->currentIndex(), m_initiative->value());
}