#pragma once
#include <QString>
#include <QMetaType>

struct CastState {
    const QString defaultText = "Spell name…";
    QString spellName = "";
    int castingTime = 1;
    int duration = 0;

    bool isDefaulted() const { return spellName == ""; };
};

Q_DECLARE_METATYPE(CastState)