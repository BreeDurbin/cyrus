#pragma once
#include "Character.h"
#include "Enums.h"

class CastAction : public Character {

    public:

    CastAction() : Character() {}

    CastAction(QString name, int initiative, Cyrus::CharacterType characterType, QString spellName, int castingTime, int duration) 
          : Character(name, initiative, characterType, Cyrus::ActionType::Cast),
            spellName_{spellName},
            castingTime_{castingTime},
            duration_{duration} {}

    CastAction(const CastAction& other) : Character(other) {}

    CastAction& operator=(const CastAction& other) {
        if (this != &other) {
            Character::operator=(other);
        }
        return *this;
    }

    int initiative() const override {
        // Total initiative before applying rounds passed
        int total = initiative_ + castingTime_;

        // Adjust based on how many rounds have passed
        int effective = total - (roundsPassed_ * 10);

        // If effective <= 0, wrap into this round’s slots (1–10)
        if (effective <= 0) {
            effective = ((effective - 1) % 10 + 10) % 10 + 1; // safe modulo 1–10
        }

        return effective;
    }

    QIcon icon() const override { return IconRepository::iconFor(actionType_); };
    QString text() const override;
    QString combatLog() const override;
    QString spellName() const { return spellName_; }
    int duration() const { return duration_ - roundsPassed_; }
    void passRound(){ ++roundsPassed_; };
    LayoutSpec layoutSpec() const override {
        return LayoutSpec{5, 12, 70, 0.8, 30, 0.6}; 
        // padding=5, radius=12, preferred height=70px, hero icon=80% height, initiative=30px, icon selector icon scale = 60%
    };

    private:
    QString spellName_{"Unknown"};
    int castingTime_{};
    int duration_{};
    int roundsPassed_{};

};
