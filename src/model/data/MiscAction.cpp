#include "MiscAction.h"
#include <QMap>

// --- Ctors ---
MiscAction::MiscAction() : Character() {}

MiscAction::MiscAction(const Character& base)
    : Character(base) {}

MiscAction::MiscAction(const MiscAction& other)
    : Character(other) {}

MiscAction& MiscAction::operator=(const MiscAction& other) {
    if (this != &other) {
        Character::operator=(other);
    }
    return *this;
}

// --- Initiative override ---
int MiscAction::initiative() const {
    return initiative_ + miscActionCost();
}

// --- Icon ---
QIcon MiscAction::icon() const {
    return IconRepository::iconFor(miscActionType_);
}

// --- Text ---
QString MiscAction::text() const {
    switch (miscActionType_) {
        case Cyrus::MiscActionType::MiscMagicItem: return name_ + " uses misc. magic item";
        case Cyrus::MiscActionType::Potion:        return name_ + " drinks a Potion";
        case Cyrus::MiscActionType::Ring:          return name_ + " uses a magic ring";
        case Cyrus::MiscActionType::Rod:           return name_ + " uses a magic rod";
        case Cyrus::MiscActionType::MagicScroll:   return name_ + " casts a spell from a scroll";
        case Cyrus::MiscActionType::Staff:         return name_ + " uses a staff";
        case Cyrus::MiscActionType::Wand:          return name_ + " uses a wand";
        case Cyrus::MiscActionType::Wait:          return name_ + " waits";
    }
    return "Unknown Action";
}

// --- Combat Log ---
QString MiscAction::combatLog() const {
    return text();
}
