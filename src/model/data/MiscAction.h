#pragma once

#include "Character.h"
#include "Enums.h"
#include "IconRepository.h"

class MiscAction : public Character {
public:
    MiscAction();
    MiscAction(const Character& base);
    MiscAction(const MiscAction& other);
    MiscAction& operator=(const MiscAction& other);

    // --- Overrides ---
    int initiative() const override;
    QIcon icon() const override;
    QString text() const override;
    QString combatLog() const override;

};
