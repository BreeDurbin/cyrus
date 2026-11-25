#pragma once
#include "Character.h"

// Combat log entry recording that a creature was removed from combat.
class RemovalEntry : public Character {

    public:

    RemovalEntry() : Character() {}

    explicit RemovalEntry(const Character& removed)
          : Character(removed) {}

    QString text() const override;
    QString combatLog() const override;

};
