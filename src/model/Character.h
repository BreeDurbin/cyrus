#pragma once
#include <QMetaType>
#include <QDebug>
#include <QVariant>
#include <initializer_list>
#include <QIcon>
#include "shared/Enums.h"
#include "style/IconRepository.h"
#include <QUuid>

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
    QString name() const { return name_; };
    int initiative() const { return initiative_; };
    Cyrus::CharacterType characterType() const { return characterType_; };
    QIcon characterIcon() const { return IconRepository::iconFor(characterType_); };
    QIcon actionIcon() const { return IconRepository::iconFor(actionType_); };
    Cyrus::ActionType actionType() const { return actionType_; };
    void setActionType(Cyrus::ActionType actionType) { actionType_ = actionType; }
    virtual QString toString() const;
    virtual QString combatLog() const;
    virtual LayoutSpec layoutSpec() const {
        return LayoutSpec{10, 12, 70, 0.9, 30, 0.6}; 
        // padding=10, radius=12, preferred height=70px hero icon=90% height, initiative=30px, icon selector icon scale = 60%
    }

private:
    QUuid uuid_{};
    QString name_{};
    int initiative_{};
    Cyrus::CharacterType characterType_{Cyrus::CharacterType::Fighter};
    Cyrus::ActionType actionType_{Cyrus::ActionType::Attack};
};

Q_DECLARE_METATYPE(Character)