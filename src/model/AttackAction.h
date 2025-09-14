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
        " attacks ",
        " casts "
    };

    AttackAction() : Character() {}

    AttackAction(QString name, int initiative, Cyrus::CharacterType characterType, Cyrus::ActionType actionType, QString actionName) 
          : Character(name, initiative, characterType, actionType),
            actionName_{actionName} {}

    AttackAction(const AttackAction& other) : Character(other) {}

    AttackAction& operator=(const AttackAction& other) {
        if (this != &other) {
            Character::operator=(other);
        }
        return *this;
    }

    QString name() const override { return actionName_; };
    int initiative() const override { return initiative_ + attackDelay_; };
    QString actionTypeLogEntry() const { return logEntryFor(actionType()); };
    QIcon icon() const override { return IconRepository::iconFor(actionType_); };
    QString text() const override;
    QString combatLog() const override;
    QString actionName() const { return actionName_; }
    LayoutSpec layoutSpec() const override {
        return LayoutSpec{5, 12, 70, 0.8, 30, 0.6, 0.8, 0.8}; 
        // padding=5, radius=12, preferred height=70px, hero icon=80% height, initiative=30px, 
        // icon selector icon scale = 60%, delete rect icon scale = 80%, submit rect icon scale = 80%
    };

    static QString logEntryFor(Cyrus::ActionType type) {
        return QString(actionTypeLogEntries.at(static_cast<int>(type)));
    }

    private:
    // to be implemented will be take from text label in initiative view current item
    QString actionName_{"Unknown"};
    int attackDelay_{};


};
