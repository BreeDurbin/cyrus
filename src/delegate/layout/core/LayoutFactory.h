// LayoutFactory.h
#pragma once

#include "LayoutEngine.h"
#include "Character.h"
#include "AttackAction.h"
#include "CastAction.h"
#include "MiscAction.h"
#include "RosterLayoutEngine.h"

class LayoutFactory {
public:
    static RosterLayoutEngine* rosterLayoutEngine();

    static LayoutEngine* forCharacter(const Character& character);

    template <typename T>
    static LayoutEngine* forCharacterType();
};

// Explicit specializations
template <>
LayoutEngine* LayoutFactory::forCharacterType<Character>();

template <>
LayoutEngine* LayoutFactory::forCharacterType<CastAction>();

template <>
LayoutEngine* LayoutFactory::forCharacterType<AttackAction>();

template <>
LayoutEngine* LayoutFactory::forCharacterType<MiscAction>();
