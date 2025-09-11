#include "CharacterAction.h"
#include <QString>

static auto dummy = QMetaType::registerConverter(&CharacterAction::toString);

QString CharacterAction::toString() const {
    return "CharacterAction::toString()  To be implented";
}

QString CharacterAction::combatLog() const {
    return name() + QString(" ") + actionTypeLogEntry() + actionName() + QString(".");
}