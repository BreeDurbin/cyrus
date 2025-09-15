#include "Character.h"
#include <QString>

QString Character::text() const { return name(); }

QString Character::combatLog() const {
    return name() + " takes their turn at initiative: " + QString::number(initiative());
}