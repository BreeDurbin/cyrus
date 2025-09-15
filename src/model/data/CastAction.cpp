#include "CastAction.h"
#include <QString>

QString CastAction::text() const { return name() + QString(" is casting ") + spellName_ + QString(" with duration ") + QString::number(duration_); }

QString CastAction::combatLog() const {
    qDebug() << "Logging: " << name_ << " cast " << "spellname " << spellName_ << " at initiative " << initiative_;
    return name_ + QString(" cast ") + spellName_ + QString(" at initiative ") + QString::number(initiative_) + QString(".");
}