#include "CastAction.h"
#include <QString>

QString CastAction::text() const { 
    qDebug() << "spellname: " << spellName_;
    if(!castingComplete_) return name() + QString(" is casting ") + spellName_;
    return name() + QString(" has cast ") + spellName_;
}

QString CastAction::combatLog() const {
    qDebug() << "Logging: " << name_ << " cast " << "spellname " << spellName_ << " at initiative " << initiative_;
    return name_ + QString(" cast ") + spellName_ + QString(" at initiative ") + QString::number(initiative_) + QString(".");
}