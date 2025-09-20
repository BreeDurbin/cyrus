#pragma once
#include <QString>

struct AttackRate {
    int halfAttacks_; // number of half attacks (e.g. 3 = 1.5 attacks/round)
    bool isAttacked_ = false;

    AttackRate(int halfAttacks = 2) : halfAttacks_(halfAttacks) {}

    // Return formatted string (e.g., "3/2" instead of "1.5")
    QString toString() const {
        if (halfAttacks_ % 2 == 0) {
            // whole number of attacks
            return QString("%1").arg(halfAttacks_ / 2);
        } else {
            // half attack
            return QString("%1/2").arg(halfAttacks_);
        }
    }

    // not a stateless function keeps track of each time an attack happens 
    // so it can account for non whole number attacks / round,  ex: 5/2 attacks a round
    int attack(){
        // whole number attack amount
        if(halfAttacks_ % 2 == 0) return halfAttacks_ / 2; 
        // extra attack from having an extra half attack;
        if(isAttacked_){
            isAttacked_ = false;
            return (halfAttacks_ / 2) + 1; 
        }
        isAttacked_ = true;
        return halfAttacks_ / 2;
    }

    // Add attacks (1.0, 0.5, etc.) with clamping
    void add(int halfAttacks) {
        halfAttacks_ += halfAttacks;
        if (halfAttacks_ < 2) halfAttacks_ = 2;
        if (halfAttacks_ > 20) halfAttacks_ = 20;
    }

    // Convenience: operator overloads
    AttackRate& operator+=(int attacks) {
        add(attacks);
        return *this;
    }

    // Prefix increment (++x): add 0.5 attack
    AttackRate& operator++() {
        if (halfAttacks_ < 20) ++halfAttacks_;
        return *this;
    }

    // Postfix increment (x++): add 0.5 attack
    AttackRate operator++(int) {
        AttackRate tmp(*this);
        ++(*this);
        return tmp;
    }

    // Prefix decrement (--x): remove 0.5 attack
    AttackRate& operator--() {
        if (halfAttacks_ > 2) --halfAttacks_;
        return *this;
    }

    // Postfix decrement (x--): remove 0.5 attack
    AttackRate operator--(int) {
        AttackRate tmp(*this);
        --(*this);
        return tmp;
    }

};
