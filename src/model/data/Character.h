#pragma once
#include <QMetaType>
#include <QDebug>
#include <QVariant>
#include <initializer_list>
#include <QIcon>
#include "Enums.h"
#include <QUuid>
#include <memory>
#include "IconRepository.h"
#include "AttackRate.h"

struct LayoutSpec {
    int padding;
    int cornerRadius;
    int preferredHeight;
    double heroIconScale;  
    int initiativeWidth;
    double iconSelectorIconScale; 
    double cloneButtonScale;
    double durationIconScale;
    double submitButtonScale;
    double deleteButtonScale;
    double initiativeIconScale;
};

class Character
{
    public:

    Character();
    Character(QString baseName,
              int initiative,
              AttackRate attackRate,
              int weaponSpeed,
              Cyrus::CharacterType characterType = Cyrus::CharacterType::Fighter,
              Cyrus::ActionType actionType = Cyrus::ActionType::Unset,
              Cyrus::Faction faction = Cyrus::Faction::Red);

    Character(QUuid uuid,
              QString baseName,
              int initiative,
              AttackRate attackRate,
              int weaponSpeed,
              Cyrus::CharacterType characterType = Cyrus::CharacterType::Fighter,
              Cyrus::ActionType actionType = Cyrus::ActionType::Unset,
              Cyrus::Faction faction = Cyrus::Faction::Red);

    // --- Rule of Five ---
    Character(const Character&) = default;
    Character(Character&&) noexcept = default;
    Character& operator=(const Character&) = default;
    Character& operator=(Character&&) noexcept = default;
    virtual ~Character() = default;

     Character clone();

    QUuid uuid() const;
    virtual QString name() const;
    virtual QString baseName() const;
    virtual int initiative() const;
    virtual void setInitiative(const int initiative);

    Cyrus::CharacterType characterType() const;
    AttackRate attackRate() const;
    void setAttackRate(AttackRate attackRate);

    int weaponSpeed() const;
    void setWeaponSpeed(int weaponSpeed);

    Cyrus::ActionType actionType() const;
    void setActionType(Cyrus::ActionType actionType);

    Cyrus::Faction faction() const;
    void setFaction(Cyrus::Faction faction);

    bool hasActed() const;
    void setActed(bool hasActed);

    Cyrus::MiscActionType miscActionType() const;
    void setMiscActionType(const Cyrus::MiscActionType miscActionType);
    int miscActionCost() const;

    int scrollCost() const;
    void setScrollCost(int scrollCost);

    int number() const { return number_; }

    virtual QIcon icon() const;
    virtual QString text() const;
    virtual QString combatLog() const;

    virtual LayoutSpec layoutSpec() const;

    static void clearNumberCache();

protected:
    // local vars
    QUuid uuid_{};
    QString name_{};
    int number_{}; // used if there are more an 1 creature with the same name
    int initiative_{};
    Cyrus::CharacterType characterType_{Cyrus::CharacterType::Fighter};
    Cyrus::ActionType actionType_{Cyrus::ActionType::Unset};
    AttackRate attackRate_{};
    int weaponSpeed_{};
    Cyrus::Faction faction_{};
    bool hasActed_{false};
    Cyrus::MiscActionType miscActionType_{Cyrus::MiscActionType::Unset};
    int scrollCost_{-1};  // Only valid for MagicScroll

    // cloning struct
    //static map of names and the amount of times they appear
    // Used for cloning
    static std::unordered_map<QString, int>& characterCount() {
        // lives on the heap, never destroyed
        static std::unordered_map<QString, int>* map = new std::unordered_map<QString, int>(); 
        return *map;
    }

    static int getCharacterCount(const QString& key) {
        auto& m = characterCount();
        auto it = m.find(key);
        return (it != m.end()) ? it->second : 0;
    }

    static void setCharacterCount(const QString& key, int value) {
        characterCount()[key] = value;
    }

    static bool characterCountContains(const QString& key) {
        return characterCount().find(key) != characterCount().end();
    }
};

Q_DECLARE_METATYPE(Character)
Q_DECLARE_METATYPE(std::shared_ptr<Character>)
Q_DECLARE_METATYPE(std::weak_ptr<Character>)