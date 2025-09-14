#pragma once

#include <QIcon>
#include <QHash>
#include "Enums.h"

namespace IconRepository {

    inline const QHash<Cyrus::CharacterType, QString> classIconPaths = {
        { Cyrus::CharacterType::Barbarian, ":/icon/class/character_icon2.png" },
        { Cyrus::CharacterType::Cleric,    ":/icon/class/character_icon3.png" },
        { Cyrus::CharacterType::Druid,     ":/icon/class/character_icon4.png" },
        { Cyrus::CharacterType::Fighter,   ":/icon/class/character_icon5.png" },
        { Cyrus::CharacterType::Monster,   ":/icon/class/Monster_Symbol.png" },
        { Cyrus::CharacterType::Paladin,   ":/icon/class/Paladin_Symbol.png" },
        { Cyrus::CharacterType::Wizard,    ":/icon/class/Wizard_Symbol.png" }
    };

    inline const QHash<Cyrus::ActionType, QString> actionIconPaths = {
        { Cyrus::ActionType::Attack, ":/icon/action/attack_icon.png" },
        { Cyrus::ActionType::Cast,   ":/icon/action/cast_icon.png" }
        // Note: None intentionally omitted
    };

    // --- Lookups ---

    inline QIcon iconFor(Cyrus::CharacterType type) {
        return QIcon(classIconPaths.value(type));
    }

    inline QIcon iconFor(Cyrus::ActionType type) {
        if (!actionIconPaths.contains(type)) return QIcon();
        return QIcon(actionIconPaths.value(type));
    }

    inline QList<QIcon> classIcons() {
        QList<QIcon> icons;
        for(auto characterType: Cyrus::orderedCharacterTypes){
            icons.append(iconFor(characterType));
        }
        return icons;
    }

    inline QList<QIcon> actionIcons() {
        QList<QIcon> icons;
        for (auto actionType : Cyrus::orderedActionTypes) {
            icons.append(iconFor(actionType));
        }
        return icons;
    }

    // UI icons
    inline QIcon miniCyrus() { return QIcon(":/image/CYRUS_ICON_TINY.png"); }
    inline QIcon save()      { return QIcon(":/icon/ui/save_icon.png"); }
    inline QIcon load()      { return QIcon(":/icon/ui/load_icon.png"); }
    inline QIcon hourglass() { return QIcon(":/icon/ui/hourglass_icon.png"); }
    inline QIcon submit()    { return QIcon(":icon/ui/submit_icon.png"); }
    inline QIcon clone()    { return QIcon(":icon/ui/clone_icon.png"); }

}
