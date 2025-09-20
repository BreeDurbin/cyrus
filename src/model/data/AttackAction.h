#pragma once
#include "Character.h"
#include "Enums.h"

class AttackAction : public Character {

    public:

    inline static const QStringList actionIconPaths {
        ":/icon/action/attack_icon.png",
        ":/icon/action/cast_icon.png"
    };

    inline static const QStringList actionTypeLogEntries {
        " attacks",
        " casts "
    };

    AttackAction() : Character() {}

    AttackAction(const Character& base, int attackNumber) 
          : Character(base),
            attackNumber_{attackNumber} {}

    AttackAction(const AttackAction& other) : Character(other) {}

    AttackAction& operator=(const AttackAction& other) {
        if (this != &other) {
            Character::operator=(other);
        }
        return *this;
    }

    int initiative() const override {
        if (attackNumber_ <= 0) return initiative_;
        return initiative_ + (attackNumber_ * weaponSpeed_);
    }
    int attackNumber() const { return attackNumber_; }
    QString actionTypeLogEntry() const { return logEntryFor(actionType()); };
    QIcon icon() const override { return IconRepository::iconFor(actionType_); };
    QString text() const override;
    QString combatLog() const override;

    static QString logEntryFor(Cyrus::ActionType type) {
        return QString(actionTypeLogEntries.at(static_cast<int>(type)));
    }

    private:
    int attackNumber_{}; // tracks which attack number this is initiative() formula is base initiative_ + (attackNumber_ * weaponSpeed_)

};
