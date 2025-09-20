// LayoutFactory.cpp
#include "LayoutFactory.h"
#include "CharacterLayoutEngine.h"
#include "CastActionLayoutEngine.h"
#include "AttackActionLayoutEngine.h"
#include "MiscActionLayoutEngine.h"
#include <typeinfo>

RosterLayoutEngine* LayoutFactory::rosterLayoutEngine(){
    static RosterLayoutEngine instance;
    return &instance;
}

LayoutEngine* LayoutFactory::forCharacter(const Character& character) {
    if (typeid(character) == typeid(CastAction)) {
        return forCharacterType<CastAction>();
    }
    if (typeid(character) == typeid(AttackAction)) {
        return forCharacterType<AttackAction>();
    }
    if (typeid(character) == typeid(MiscAction)) {
        return forCharacterType<MiscAction>();
    }
    return forCharacterType<Character>();
}

// Specializations
template <>
LayoutEngine* LayoutFactory::forCharacterType<Character>() {
    static CharacterLayoutEngine instance;
    return &instance;
}

template <>
LayoutEngine* LayoutFactory::forCharacterType<CastAction>() {
    static CastActionLayoutEngine instance;
    return &instance;
}

template <>
LayoutEngine* LayoutFactory::forCharacterType<MiscAction>() {
    static MiscActionLayoutEngine instance;
    return &instance;
}

template <>
LayoutEngine* LayoutFactory::forCharacterType<AttackAction>() {
    static AttackActionLayoutEngine instance;
    return &instance;
}
