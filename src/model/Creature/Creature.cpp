#include "Creature.h"
#include <QString>

int Creature::qtMetaType = qRegisterMetaType<Creature>();
static auto dummy = QMetaType::registerConverter(&Creature::toString);


Creature::Creature(QString name, Role role, int initiative)
    : m_name(name)
    , m_role(role)
    , m_initiative(initiative)
{
}

QString Creature::name() const
{
    return m_name;
}

Creature::Role Creature::role() const
{
    return m_role;
}

int Creature::initiative() const
{
    return m_initiative;
}

QString Creature::toString() const
{
    return QString("Name: %1 Role: %2 Initiative: %3").arg(m_name).arg(m_role).arg(m_initiative);
}