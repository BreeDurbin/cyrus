#include"Character.h"
#include "../helper/Enums.h"

class CharacterAction : public Character {

    public:

    inline static const QStringList actionIconPaths {
        ":/icon/action/attack_icon.png",
        ":/icon/action/cast_icon.png"
    };

    inline static const QStringList actionTypeLogEntries {
        " attacks ",
        " casts "
    };

    CharacterAction() : Character() {}

    CharacterAction(QString name, int initiative, Cyrus::CharacterType characterType, Cyrus::ActionType actionType) 
          : Character(name, initiative, characterType, actionType) {}

    CharacterAction(const CharacterAction& other) : Character(other) {}

    CharacterAction& operator=(const CharacterAction& other) {
        if (this != &other) {
            Character::operator=(other);
        }
        return *this;
    }

    QString actionTypeLogEntry() const { return logEntryFor(actionType()); };
    QString toString() const override;
    QString combatLog() const override;
    QString actionName() const { return actionName_; }
    LayoutSpec layoutSpec() const override {
        return LayoutSpec{5, 12, 70, 0.8, 30, 0.6}; 
        // padding=5, radius=12, preferred height=70px, hero icon=80% height, initiative=30px, icon selector icon scale = 60%
    };

    static QString logEntryFor(Cyrus::ActionType type) {
        return QString(actionTypeLogEntries.at(static_cast<int>(type)));
    }

    private:
    // to be implemented will be take from text label in initiative view current item
    QString actionName_{"Unknown"};


};
