#include "AttackAction.h"

#include <QString>


 QString AttackAction::text() const {
    return "CharacterAction::text()  To be implented";
}

QString AttackAction::combatLog() const {
    return name() + QString(" ") + actionTypeLogEntry() + actionName() + QString(".");
}