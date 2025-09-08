#include "Character.h"
#include <QString>

static auto dummy = QMetaType::registerConverter(&Character::toString);

QString Character::toString() const
{
    std::string str = ": " + std::to_string(initiative_);
    return name() + QString::fromStdString( str );
}

QString Character::combatLog() const {
    return name() + " takes their turn at initiative: " + QString::number(initiative());
}