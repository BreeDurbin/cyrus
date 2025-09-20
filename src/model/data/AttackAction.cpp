#include "AttackAction.h"

#include <QString>


QString AttackAction::text() const {
    return name() + " attack " + QString::number(attackNumber_);
}

QString AttackAction::combatLog() const {
    return name() + QString(" ") + actionTypeLogEntry() + QString(".");
}