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
        double deleteButtonScale;
        double submitButtonScale;
    };

    Character()
        : uuid_(QUuid::createUuid()), name_{}, initiative_{0},
        characterType_{Cyrus::CharacterType::Fighter},
        actionType_{Cyrus::ActionType::Attack} {  }

    Character(QString name, int initiative, Cyrus::CharacterType characterType = Cyrus::CharacterType::Fighter, Cyrus::ActionType actionType = Cyrus::ActionType::None) 
          : uuid_(QUuid::createUuid()),
            name_{std::move(name)}, 
            initiative_{initiative}, 
            characterType_{characterType},
            actionType_{actionType} {  }

    Character(QUuid uuid, QString name, int initiative, Cyrus::CharacterType characterType = Cyrus::CharacterType::Fighter, Cyrus::ActionType actionType = Cyrus::ActionType::None) 
          : uuid_(uuid),
            name_{std::move(name)}, 
            initiative_{initiative}, 
            characterType_{characterType},
            actionType_{actionType} {  }


    Character(const Character& other) 
        : uuid_(other.uuid_),   // keep the same id
        name_{other.name()}, 
        initiative_{other.initiative()}, 
        characterType_{other.characterType()},
        actionType_{other.actionType()} {  }

    Character& operator=(const Character& other) {
        if (this != &other) {
            uuid_ = other.uuid_;   // keep the same id
            name_ = other.name();
            initiative_ = other.initiative();
            characterType_ = other.characterType();
            actionType_ = other.actionType();
        }
        return *this;
    }

    Character clone() const {
        return Character (this->name_, this->initiative_, this->characterType_, this->actionType_);
    }


    QUuid uuid() const { return uuid_; };
    virtual QString name() const { return name_; };
    virtual int initiative() const { return initiative_; };
    virtual void setInitiative(const int initiative) { initiative_ = initiative; };
    Cyrus::CharacterType characterType() const { return characterType_; };
    Cyrus::ActionType actionType() const { return actionType_; }
    void setActionType(Cyrus::ActionType actionType) { actionType_ = actionType; }
    virtual QIcon icon() const { return IconRepository::iconFor(characterType_); };
    virtual QString text() const;
    virtual QString combatLog() const;
    virtual LayoutSpec layoutSpec() const {
        return LayoutSpec{10, 12, 70, 0.9, 30, 0.6, 0.6, 0.6}; 
        // padding=10, radius=12, preferred height=70px hero icon=90% height, initiative=30px, 
        // icon selector icon scale = 60%, delete rect icon scale = 80%, submit rect icon scale = 80%
    }

protected:
    QUuid uuid_{};
    QString name_{};
    int initiative_{};
    Cyrus::CharacterType characterType_{Cyrus::CharacterType::Fighter};
    Cyrus::ActionType actionType_{Cyrus::ActionType::Attack};
};

Q_DECLARE_METATYPE(Character)
Q_DECLARE_METATYPE(std::shared_ptr<Character>)
Q_DECLARE_METATYPE(std::weak_ptr<Character>)