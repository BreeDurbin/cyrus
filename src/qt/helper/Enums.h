#pragma once

namespace Cyrus {

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
        None
    };

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
        ActionType::Cast
        // Add new ones here in desired UI order
    };

}