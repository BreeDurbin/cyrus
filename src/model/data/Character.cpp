#include "Character.h"
#include <QString>

#include "Character.h"

// locals

// --- Cost lookup table (except scrolls) ---
static const QMap<Cyrus::MiscActionType, int> kMiscActionCosts = {
    {Cyrus::MiscActionType::MiscMagicItem, 3},
    {Cyrus::MiscActionType::Potion,        4},
    {Cyrus::MiscActionType::Ring,          3},
    {Cyrus::MiscActionType::Rod,           1},
    {Cyrus::MiscActionType::Staff,         2},
    {Cyrus::MiscActionType::Wand,          3},
    {Cyrus::MiscActionType::Wait,          0}
};

// constructors

// Default constructor
Character::Character()
    : uuid_(QUuid::createUuid()),
      name_{},
      initiative_{0},
      characterType_{Cyrus::CharacterType::Fighter},
      actionType_{Cyrus::ActionType::Attack}
{
}

// Constructor with baseName
Character::Character(QString baseName,
                     int initiative,
                     AttackRate attackRate,
                     int weaponSpeed,
                     Cyrus::CharacterType characterType,
                     Cyrus::ActionType actionType,
                     Cyrus::Faction faction)
    : uuid_(QUuid::createUuid()),
      name_{std::move(baseName)},
      initiative_{initiative},
      attackRate_{attackRate},
      weaponSpeed_{weaponSpeed},
      characterType_{characterType},
      actionType_{actionType},
      faction_{faction}
{
    number_ = getCharacterCount(name_) + 1;
    setCharacterCount(name_, number_);
}

// Constructor with explicit UUID
Character::Character(QUuid uuid,
                     QString baseName,
                     int initiative,
                     AttackRate attackRate,
                     int weaponSpeed,
                     Cyrus::CharacterType characterType,
                     Cyrus::ActionType actionType,
                     Cyrus::Faction faction)
    : uuid_(uuid),
      name_{std::move(baseName)},
      initiative_{initiative},
      attackRate_{attackRate},
      weaponSpeed_{weaponSpeed},
      characterType_{characterType},
      actionType_{actionType},
      faction_{faction}
{
    number_ = getCharacterCount(name_) + 1;
    setCharacterCount(name_, number_);
}


// Clone
Character Character::clone() {
    if (number_ == 0) {
        number_ = 1;
        setCharacterCount(name_, 1);
    }

    Character c(this->name_, this->initiative_, this->attackRate_,
                this->weaponSpeed_, this->characterType_,
                this->actionType_, this->faction_);

    return c;
}



// Simple getters/setters
QUuid Character::uuid() const { return uuid_; }

QString Character::name() const {
    if (getCharacterCount(name_) == 1) return name_;
    return name_ + QString(" ") + QString::number(number_);
}

QString Character::baseName() const { return name_; }

int Character::initiative() const { return initiative_; }
void Character::setInitiative(const int initiative) { initiative_ = initiative; }

Cyrus::CharacterType Character::characterType() const { return characterType_; }

Cyrus::Faction Character::faction() const { return faction_; }
void Character::setFaction(Cyrus::Faction faction) { faction_ = faction; }

bool Character::hasActed() const { return hasActed_; }
void Character::setActed(bool hasActed) { hasActed_ = hasActed; }

// Actions
AttackRate Character::attackRate() const { return attackRate_; }
void Character::setAttackRate(AttackRate attackRate) { attackRate_ = attackRate; }

int Character::weaponSpeed() const { return weaponSpeed_; }
void Character::setWeaponSpeed(int weaponSpeed) { weaponSpeed_ = weaponSpeed; }

Cyrus::ActionType Character::actionType() const { return actionType_; }
void Character::setActionType(Cyrus::ActionType actionType) { actionType_ = actionType; }

Cyrus::MiscActionType Character::miscActionType() const { return miscActionType_; }

void Character::setMiscActionType(const Cyrus::MiscActionType miscActionType) { miscActionType_ = miscActionType; }

// --- Cost accessor ---
int Character::miscActionCost() const {
    if (miscActionType_ == Cyrus::MiscActionType::MagicScroll) {
        return (scrollCost_ > 0) ? scrollCost_ : 0;
    }
    return kMiscActionCosts.value(miscActionType_, 0);
}

void Character::setScrollCost(int scrollCost) {
    scrollCost_ = scrollCost;
}

int Character::scrollCost() const {
    return scrollCost_;
}

// UI-facing
QIcon Character::icon() const {
    return IconRepository::iconFor(characterType_);
}

QString Character::text() const {
    return name();
}

QString Character::combatLog() const {
    return name() + " takes their turn at initiative: " + QString::number(initiative());
}

LayoutSpec Character::layoutSpec() const {
    LayoutSpec layout;
    layout.padding = 7;
    layout.cornerRadius = 12;
    layout.preferredHeight = 70;
    layout.heroIconScale = 1.1;  
    layout.initiativeWidth = 30;
    layout.iconSelectorIconScale = 0.65; 
    layout.cloneButtonScale = 0.65;
    layout.durationIconScale = 0.65;
    layout.submitButtonScale = 0.65;
    layout.deleteButtonScale = 0.65;
    layout.initiativeIconScale = 0.65;
    return layout;
}

// Static helpers
void Character::clearNumberCache() {
    characterCount().clear();
}



