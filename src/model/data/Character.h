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

class Character
{
    public:

    struct LayoutSpec {
        int padding;
        int cornerRadius;
        int preferredHeight;
        double heroIconScale;               // fraction of row height
        int initiativeWidth;
        double iconSelectorIconScale;       // fraction of row height
        double cloneButtonScale;
        double submitButtonScale;
        double deleteButtonScale;
    };

    Character()
        : uuid_(QUuid::createUuid()), name_{}, initiative_{0},
        characterType_{Cyrus::CharacterType::Fighter},
        actionType_{Cyrus::ActionType::Attack} {  }

    Character(QString name, int initiative, Cyrus::CharacterType characterType = Cyrus::CharacterType::Fighter,
            Cyrus::ActionType actionType = Cyrus::ActionType::None) 
            : uuid_(QUuid::createUuid()),
            name_{std::move(name)},
            initiative_{initiative},
            characterType_{characterType},
            actionType_{actionType}{
        number_ = getCharacterCount(name_) + 1;
        setCharacterCount(name_, number_);
    }


    Character(QUuid uuid, QString name, int initiative, Cyrus::CharacterType characterType = Cyrus::CharacterType::Fighter, 
            Cyrus::ActionType actionType = Cyrus::ActionType::None) 
          : uuid_(QUuid::createUuid()),
            name_{std::move(name)}, 
            initiative_{initiative}, 
            characterType_{characterType},
            actionType_{actionType} {
        number_ = getCharacterCount(name_) + 1;
        setCharacterCount(name_, number_);
    }


    Character(const Character& other) 
        :   uuid_(other.uuid_),   // keep the same id
            name_{other.baseName()}, 
            initiative_{other.initiative()}, 
            characterType_{other.characterType()},
            actionType_{other.actionType()},
            number_{other.number()} {  }

    Character& operator=(const Character& other) {
        if (this != &other) {
            uuid_ = other.uuid_;   // keep the same id
            name_ = other.baseName();
            initiative_ = other.initiative();
            characterType_ = other.characterType();
            actionType_ = other.actionType();
            number_ = other.number();
        }
        return *this;
    }

    Character clone() {
        // If this is the very first time we duplicate a character of this name,
        // assign number 1 to the original
        if (number_ == 0) {
            number_ = 1;
            setCharacterCount(name_, 1);
        }

        Character c(this->name_, this->initiative_, this->characterType_, this->actionType_);
        return c;
    }

    QUuid uuid() const { return uuid_; }
    virtual QString name() const {
        if(getCharacterCount(name_) == 1) return name_;
        return name_ + QString(" ") + QString::number(number_);
    }
    virtual QString baseName() const { return name_; }
    int number() const { return number_; }
    virtual int initiative() const { return initiative_; }
    virtual void setInitiative(const int initiative) { initiative_ = initiative; }
    Cyrus::CharacterType characterType() const { return characterType_; }
    Cyrus::ActionType actionType() const { return actionType_; }
    AttackRate attackRate() const { return attackRate_; }
    void setAttackRate(AttackRate attackRate) { attackRate_ = attackRate; }
    void setActionType(Cyrus::ActionType actionType) { actionType_ = actionType; }
    virtual QIcon icon() const { return IconRepository::iconFor(characterType_); }
    virtual QString text() const;
    virtual QString combatLog() const;
    virtual LayoutSpec layoutSpec() const {
        return LayoutSpec{10, 12, 70, 0.9, 30, 0.6, 0.6, 0.6, 0.6}; 
        // padding=10, radius=12, preferred height=70px hero icon=90% height, initiative=30px, 
        // icon selector icon scale = 60%, clone rect icon scale = 80%, submit rect icon scale = 80%, delete rect icon scale = 80%
    }

protected:
    QUuid uuid_{};
    QString name_{};
    int number_{}; // ussed if there are more an 1 creature with the same name
    int initiative_{};
    Cyrus::CharacterType characterType_{Cyrus::CharacterType::Fighter};
    Cyrus::ActionType actionType_{Cyrus::ActionType::Attack};
    AttackRate attackRate_;


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