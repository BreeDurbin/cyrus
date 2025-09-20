#pragma once

#include <QIcon>
#include <QHash>
#include "Enums.h"

namespace IconRepository {

    inline const QHash<Cyrus::CharacterType, QString> classIconPaths = {
        { Cyrus::CharacterType::Barbarian, ":/icon/class/character_icon1.png" },
        { Cyrus::CharacterType::Cleric,    ":/icon/class/character_icon3.png" },
        { Cyrus::CharacterType::Druid,     ":/icon/class/character_icon4.png" },
        { Cyrus::CharacterType::Fighter,   ":/icon/class/character_icon5.png" },
        { Cyrus::CharacterType::Monster,   ":/icon/class/character_icon2.png" },
        { Cyrus::CharacterType::Paladin,   ":/icon/class/Paladin_Symbol.png" },
        { Cyrus::CharacterType::Wizard,    ":/icon/class/Wizard_Symbol.png" }
    };

    inline const QHash<Cyrus::MiscActionType, QString> miscActionIconPaths = {
        { Cyrus::MiscActionType::MiscMagicItem,  ":/icon/action/amulet_icon.png" },
        { Cyrus::MiscActionType::Potion,         ":/icon/action/potion_icon.png" },
        { Cyrus::MiscActionType::Ring,           ":/icon/action/ring_icon.png" },
        { Cyrus::MiscActionType::Rod,            ":/icon/action/rod_icon.png" },
        { Cyrus::MiscActionType::MagicScroll,    ":/icon/action/magic_scroll_icon.png" },
        { Cyrus::MiscActionType::Staff,          ":/icon/action/staff_icon.png" },
        { Cyrus::MiscActionType::Wand,           ":/icon/action/wand_icon.png" },
        { Cyrus::MiscActionType::Wait,           ":/icon/action/hourglass_icon.png" }
    };

    inline const QHash<Cyrus::ActionType, QString> actionIconPaths = {
        { Cyrus::ActionType::Attack, ":/icon/action/attack_icon.png" },
        { Cyrus::ActionType::Cast,   ":/icon/action/cast_icon.png" },
        { Cyrus::ActionType::Misc,   ":/icon/action/misc_icon.png" }
    };

    // --- Lookups ---

    inline QIcon iconFor(Cyrus::CharacterType type) {
        static QHash<Cyrus::CharacterType, QIcon> cache;
        if (!cache.contains(type)) {
            cache[type] = QIcon(classIconPaths.value(type));
        }
        return cache[type];
    }

    inline QIcon iconFor(Cyrus::ActionType type) {
        static QHash<Cyrus::ActionType, QIcon> cache;
        if (!cache.contains(type) && actionIconPaths.contains(type)) {
            cache[type] = QIcon(actionIconPaths.value(type));
        }
        return cache.value(type);
    }

    inline QIcon iconFor(Cyrus::MiscActionType type) {
        static QHash<Cyrus::MiscActionType, QIcon> cache;
        if (!cache.contains(type) && miscActionIconPaths.contains(type)) {
            cache[type] = QIcon(miscActionIconPaths.value(type));
        }
        return cache.value(type);
    }

    inline QList<QIcon> classIcons() {
        static QList<QIcon> icons;
        if (icons.isEmpty()) {
            for (auto characterType : Cyrus::orderedCharacterTypes) {
                icons.append(iconFor(characterType));
            }
        }
        return icons;
    }

    inline QList<QIcon> actionIcons() {
        static QList<QIcon> icons;
        if (icons.isEmpty()) {
            for (auto actionType : Cyrus::orderedActionTypes) {
                icons.append(iconFor(actionType));
            }
        }
        return icons;
    }

    inline QList<QIcon> miscActionIcons() {
        static QList<QIcon> icons;
        if (icons.isEmpty()) {
            for (auto miscActionType : Cyrus::orderedMiscActionTypes) {
                icons.append(iconFor(miscActionType));
            }
        }
        return icons;
    }

    inline QIcon miniCyrus_icon() {
        static const QIcon icon(":/image/CYRUS_ICON_TINY.png");
        return icon;
    }

    inline QIcon save_icon() {
        static const QIcon icon(":/icon/ui/save_icon.png");
        return icon;
    }

    inline QIcon load_icon() {
        static const QIcon icon(":/icon/ui/load_icon.png");
        return icon;
    }

    inline QIcon hourglass_icon() {
        static const QIcon icon(":/icon/action/hourglass_icon.png");
        return icon;
    }

    inline QIcon submit_icon() {
        static const QIcon icon(":/icon/ui/submit_icon.png");
        return icon;
    }

    inline QIcon clone_icon() {
        static const QIcon icon(":/icon/ui/clone_icon.png");
        return icon;
    }

    inline QIcon cancel_icon() {
        static const QIcon icon(":/icon/ui/cancel_icon.png");
        return icon;
    }

    inline QIcon delete_icon() {
        static const QIcon icon(":/icon/ui/delete_icon.png");
        return icon;
    }

    inline QIcon initiative_icon() {
        static const QIcon icon(":/icon/ui/initiative_icon.png");
        return icon;
    }

    inline QIcon left_arrow_icon(bool isDark = true) {
        static QIcon dark(":icon/ui/left_arrow_dark_icon.png");
        static QIcon light(":icon/ui/left_arrow_light_icon.png");
        return isDark ? dark : light;
    }

    inline QIcon right_arrow_icon(bool isDark = true) {
        static QIcon dark(":icon/ui/right_arrow_dark_icon.png");
        static QIcon light(":icon/ui/right_arrow_light_icon.png");
        return isDark ? dark : light;
    }
}
