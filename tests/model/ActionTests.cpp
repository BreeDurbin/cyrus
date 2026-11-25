#include "TestHelpers.h"

#include "AttackAction.h"
#include "CastAction.h"
#include "MiscAction.h"
#include "RemovalEntry.h"

using namespace Cyrus;
using TestHelpers::makeCharacter;

// ---- AttackAction ----

TEST_CASE("AttackAction resolves at initiative + attack number * weapon speed", "[model][AttackAction]") {
    // README example: Ragnar, initiative 3, weapon speed 4 -> attacks at 7 and 11.
    auto ragnar = makeCharacter("Ragnar", 3, Faction::Blue, 5, 4);

    AttackAction first(*ragnar, 1);
    AttackAction second(*ragnar, 2);

    CHECK(first.initiative() == 7);
    CHECK(second.initiative() == 11);
    CHECK(first.attackNumber() == 1);
    CHECK(second.attackNumber() == 2);
}

TEST_CASE("AttackAction with no attack number uses base initiative", "[model][AttackAction]") {
    auto ragnar = makeCharacter("Ragnar", 3, Faction::Blue, 2, 4);
    CHECK(AttackAction(*ragnar, 0).initiative() == 3);
}

TEST_CASE("AttackAction shares the character's identity", "[model][AttackAction]") {
    auto ragnar = makeCharacter("Ragnar", 3, Faction::Blue, 2, 4);
    AttackAction attack(*ragnar, 1);

    CHECK(attack.uuid() == ragnar->uuid());
    CHECK(attack.name() == ragnar->name());
    CHECK(attack.faction() == Faction::Blue);
}

TEST_CASE("AttackAction text names the attack", "[model][AttackAction]") {
    auto ragnar = makeCharacter("Ragnar", 3, Faction::Blue, 2, 4);
    AttackAction attack(*ragnar, 2);
    CHECK(attack.text() == QString("Ragnar attack 2"));
}

TEST_CASE("Copying an AttackAction keeps its attack number", "[model][AttackAction]") {
    // Regression: hand-written copy operations used to copy only the Character
    // base, resetting attackNumber_ to 0 so the copy resolved at base initiative.
    auto ragnar = makeCharacter("Ragnar", 3, Faction::Blue, 2, 4);
    AttackAction original(*ragnar, 2);

    AttackAction copy(original);
    CHECK(copy.attackNumber() == 2);
    CHECK(copy.initiative() == 11);

    AttackAction assigned;
    assigned = original;
    CHECK(assigned.attackNumber() == 2);
    CHECK(assigned.initiative() == 11);
    CHECK(assigned.uuid() == original.uuid());
}

TEST_CASE("Moving an AttackAction keeps its attack number", "[model][AttackAction]") {
    auto ragnar = makeCharacter("Ragnar", 3, Faction::Blue, 2, 4);
    AttackAction original(*ragnar, 2);

    AttackAction moved(std::move(original));
    CHECK(moved.attackNumber() == 2);
    CHECK(moved.initiative() == 11);
}

// ---- CastAction ----

TEST_CASE("CastAction resolves after its casting time", "[model][CastAction]") {
    auto wizard = makeCharacter("Elminster", 3);
    CastAction fireball(*wizard, "Fireball", 3, 0);

    CHECK(fireball.initiative() == 6);
    CHECK(fireball.spellName() == QString("Fireball"));
    CHECK(fireball.uuid() == wizard->uuid());
}

TEST_CASE("CastAction keeps its segment in later rounds", "[model][CastAction]") {
    auto wizard = makeCharacter("Elminster", 3);
    CastAction shield(*wizard, "Shield", 2, 10);
    REQUIRE(shield.initiative() == 5);

    for (int round = 1; round <= 3; ++round) {
        shield.passRound();
        INFO("after round " << round);
        CHECK(shield.initiative() == 5);
    }
}

TEST_CASE("CastAction casting past segment 10 lands in the next round", "[model][CastAction]") {
    auto wizard = makeCharacter("Elminster", 8);
    CastAction spell(*wizard, "Slow Spell", 5, 3);

    CHECK(spell.initiative() == 13);
    spell.passRound();
    CHECK(spell.initiative() == 3);
}

TEST_CASE("CastAction duration counts down each round", "[model][CastAction]") {
    // README example: a 10 round shield spell counts down once cast.
    auto wizard = makeCharacter("Elminster", 3);
    CastAction shield(*wizard, "Shield", 1, 10);

    CHECK(shield.duration() == 10);
    shield.passRound();
    CHECK(shield.duration() == 9);
    shield.passRound();
    CHECK(shield.duration() == 8);
}

TEST_CASE("CastAction text reflects casting progress", "[model][CastAction]") {
    auto wizard = makeCharacter("Elminster", 3);
    CastAction shield(*wizard, "Shield", 1, 10);

    CHECK(shield.text() == QString("Elminster is casting Shield"));
    shield.passRound();
    CHECK(shield.text() == QString("Elminster has cast Shield"));
}

TEST_CASE("CastAction combat log records the cast", "[model][CastAction]") {
    auto wizard = makeCharacter("Elminster", 3);
    CastAction fireball(*wizard, "Fireball", 3, 0);
    CHECK(fireball.combatLog() == QString("Elminster cast Fireball at initiative 3."));
}

// ---- MiscAction ----

TEST_CASE("MiscAction resolves at initiative + item cost", "[model][MiscAction]") {
    auto thief = makeCharacter("Mouse", 4);

    thief->setMiscActionType(MiscActionType::Potion);
    CHECK(MiscAction(*thief).initiative() == 8);

    thief->setMiscActionType(MiscActionType::Rod);
    CHECK(MiscAction(*thief).initiative() == 5);

    thief->setMiscActionType(MiscActionType::MagicScroll);
    thief->setScrollCost(3);
    CHECK(MiscAction(*thief).initiative() == 7);
}

TEST_CASE("Copying a MiscAction keeps its item and cost", "[model][MiscAction]") {
    auto thief = makeCharacter("Mouse", 4);
    thief->setMiscActionType(MiscActionType::MagicScroll);
    thief->setScrollCost(3);
    MiscAction original(*thief);

    MiscAction copy(original);
    MiscAction assigned;
    assigned = original;

    for (const MiscAction* action : {&copy, &assigned}) {
        CHECK(action->miscActionType() == MiscActionType::MagicScroll);
        CHECK(action->initiative() == 7);
        CHECK(action->uuid() == thief->uuid());
    }
}

TEST_CASE("MiscAction text describes the item used", "[model][MiscAction]") {
    auto thief = makeCharacter("Mouse", 4);

    const std::vector<std::pair<MiscActionType, QString>> texts = {
        {MiscActionType::MiscMagicItem, "Mouse uses misc. magic item"},
        {MiscActionType::Potion,        "Mouse drinks a Potion"},
        {MiscActionType::Ring,          "Mouse uses a magic ring"},
        {MiscActionType::Rod,           "Mouse uses a magic rod"},
        {MiscActionType::MagicScroll,   "Mouse casts a spell from a scroll"},
        {MiscActionType::Staff,         "Mouse uses a staff"},
        {MiscActionType::Wand,          "Mouse uses a wand"},
        {MiscActionType::Wait,          "Mouse waits"},
    };
    for (const auto& [type, text] : texts) {
        thief->setMiscActionType(type);
        MiscAction action(*thief);
        CHECK(action.text() == text);
        CHECK(action.combatLog() == text);
    }
}

// ---- RemovalEntry ----

TEST_CASE("RemovalEntry logs that the creature was removed", "[model][RemovalEntry]") {
    auto orc = makeCharacter("Orc", 7, Faction::Red);
    RemovalEntry entry(*orc);

    CHECK(entry.combatLog() == QString("Orc was removed from combat."));
    CHECK(entry.text() == entry.combatLog());
    CHECK(entry.uuid() == orc->uuid());
    CHECK(entry.characterType() == orc->characterType());
    CHECK(entry.faction() == Faction::Red);
}

TEST_CASE("RemovalEntry uses the numbered name", "[model][RemovalEntry]") {
    auto first = makeCharacter("Goblin", 1);
    auto second = makeCharacter("Goblin", 1);
    CHECK(RemovalEntry(*second).combatLog() == QString("Goblin 2 was removed from combat."));
}
