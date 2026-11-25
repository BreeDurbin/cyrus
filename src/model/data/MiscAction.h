#pragma once

#include "Character.h"
#include "Enums.h"
#include "IconRepository.h"

class MiscAction : public Character {
public:
    MiscAction();
    MiscAction(const Character& base);

    // --- Overrides ---
    int initiative() const override;
    QIcon icon() const override;
    QString text() const override;
    QString combatLog() const override;

};
