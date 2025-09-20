#pragma once
#include "Character.h"
#include "Enums.h"

class CastAction : public Character {

    public:

    CastAction() : Character() {}

    CastAction(const Character& base, QString spellName, int castingTime, int duration) 
          : Character(base),
            spellName_{spellName},
            castingTime_{castingTime},
            duration_{duration} {}


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
    void setCastingFinished() { castingComplete_ = true; }
    int duration() const { return duration_ - roundsPassed_; }
    void passRound(){ 
        ++roundsPassed_; 
        if(!castingComplete_) setCastingFinished(); 
    };
    
    private:
    QString spellName_{"Unknown"};
    int castingTime_{};
    int duration_{};
    int roundsPassed_{};
    bool castingComplete_{false};

};
