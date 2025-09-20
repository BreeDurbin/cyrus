#pragma once

namespace Cyrus {

    enum class CombatSequenceState {
        NPC_DETERMINATION,
        PLAYER_DETERMINATION,
        INITIATIVE,
        RESOLUTION
    };

    enum ModelRoles { 
        CharacterRole = Qt::UserRole + 1, 
        InitiativeRole,
        RoleTypeRole,
        SpellNameEditRole,
        ActionNameEditRole
    };

    enum class CharacterType {
        Barbarian,
        Cleric,
        Druid,
        Fighter,
        Monster,
        Paladin,
        Wizard
    };

    enum class ActionType {
        Attack,
        Cast,
        Misc,
        Unset
    };

    enum class MiscActionType {
        MiscMagicItem,
        Potion,
        Ring,
        Rod,
        MagicScroll, 
        Staff,
        Wand, 
        Wait,
        Unset
    };

    enum class Faction {
        Red,
        Blue,
        Gray,
        Orange,
        Yellow,
        Green,
        Purple,
        Count // sentinel value to know the num of entries in the enum, for mod wrap around
    };

    // Convert enum to int
    constexpr int toInt(Faction f) {
        return static_cast<int>(f);
    }

    // Convert int to enum
    constexpr Faction toFaction(int i) {
        return static_cast<Faction>(i);
    }

    inline Faction nextFaction(Faction f) {
        return toFaction((toInt(f) + 1) % toInt(Faction::Count));
    }

    // Cycle backward
    inline Faction prevFaction(Faction f) {
        int i = toInt(f) - 1;
        if (i < 0) i = toInt(Faction::Count) - 1;
        return toFaction(i);
    }

    inline const QList<CharacterType> orderedCharacterTypes = {
        CharacterType::Barbarian,
        CharacterType::Cleric,
        CharacterType::Druid,
        CharacterType::Fighter,
        CharacterType::Monster,
        CharacterType::Paladin,
        CharacterType::Wizard
    };

    inline const QList<ActionType> orderedActionTypes = {
        ActionType::Attack,
        ActionType::Cast,
        ActionType::Misc
        // Add new ones here in desired UI order
    };

    inline const QList<MiscActionType> orderedMiscActionTypes = {
        MiscActionType::MiscMagicItem,
        MiscActionType::Potion,
        MiscActionType::Ring,
        MiscActionType::Rod,
        MiscActionType::MagicScroll, 
        MiscActionType::Staff,
        MiscActionType::Wand, 
        MiscActionType::Wait 
    };

}