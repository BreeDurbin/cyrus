#include "TestHelpers.h"

#include "Character.h"

using namespace Cyrus;
using TestHelpers::makeCharacter;

TEST_CASE("Character stores its constructor values", "[model][Character]") {
    Character c("Ragnar", 3, AttackRate{5}, 4,
                CharacterType::Fighter, ActionType::Attack, Faction::Blue);

    CHECK(c.baseName() == QString("Ragnar"));
    CHECK(c.initiative() == 3);
    CHECK(c.attackRate().halfAttacks_ == 5);
    CHECK(c.weaponSpeed() == 4);
    CHECK(c.characterType() == CharacterType::Fighter);
    CHECK(c.actionType() == ActionType::Attack);
    CHECK(c.faction() == Faction::Blue);
    CHECK_FALSE(c.hasActed());
    CHECK_FALSE(c.isSentinel());
    CHECK_FALSE(c.uuid().isNull());
}

TEST_CASE("Character with an explicit uuid keeps it", "[model][Character]") {
    const QUuid id = QUuid::createUuid();
    Character c(id, "Ragnar", 3, AttackRate{2}, 4);
    CHECK(c.uuid() == id);
}

TEST_CASE("Characters get distinct uuids", "[model][Character]") {
    auto a = makeCharacter("Orc", 1);
    auto b = makeCharacter("Goblin", 1);
    CHECK(a->uuid() != b->uuid());
}

TEST_CASE("Character names are numbered only when the base name repeats", "[model][Character]") {
    auto first = makeCharacter("Drow Fighter", 1);
    CHECK(first->name() == QString("Drow Fighter"));

    auto second = makeCharacter("Drow Fighter", 1);
    CHECK(first->name() == QString("Drow Fighter 1"));
    CHECK(second->name() == QString("Drow Fighter 2"));
    CHECK(second->number() == 2);

    auto other = makeCharacter("Priestess", 1);
    CHECK(other->name() == QString("Priestess"));
}

TEST_CASE("Character::clearNumberCache restarts numbering", "[model][Character]") {
    makeCharacter("Orc", 1);
    makeCharacter("Orc", 1);
    Character::clearNumberCache();

    auto fresh = makeCharacter("Orc", 1);
    CHECK(fresh->number() == 1);
    CHECK(fresh->name() == QString("Orc"));
}

TEST_CASE("Character::clone copies stats with a new uuid and the next number", "[model][Character]") {
    Character original("Drow Fighter", 4, AttackRate{3}, 5,
                       CharacterType::Fighter, ActionType::Attack, Faction::Gray);

    Character copy = original.clone();

    CHECK(copy.uuid() != original.uuid());
    CHECK(copy.baseName() == original.baseName());
    CHECK(copy.initiative() == 4);
    CHECK(copy.attackRate().halfAttacks_ == 3);
    CHECK(copy.weaponSpeed() == 5);
    CHECK(copy.characterType() == CharacterType::Fighter);
    CHECK(copy.faction() == Faction::Gray);

    CHECK(original.name() == QString("Drow Fighter 1"));
    CHECK(copy.name() == QString("Drow Fighter 2"));
}

TEST_CASE("Cloning four times numbers five monsters", "[model][Character]") {
    // README example: add a drow fighter, clone 4 times, get 5 numbered fighters.
    Character original("Drow Fighter", 1, AttackRate{2}, 5);
    std::vector<Character> clones;
    for (int i = 0; i < 4; ++i) clones.push_back(original.clone());

    CHECK(original.name() == QString("Drow Fighter 1"));
    for (int i = 0; i < 4; ++i) {
        CHECK(clones[i].name() == QString("Drow Fighter %1").arg(i + 2));
    }
}

TEST_CASE("Character setters update state", "[model][Character]") {
    auto c = makeCharacter("Ragnar", 3);

    c->setInitiative(7);
    c->setAttackRate(AttackRate{4});
    c->setWeaponSpeed(9);
    c->setActionType(ActionType::Cast);
    c->setFaction(Faction::Purple);
    c->setActed(true);

    CHECK(c->initiative() == 7);
    CHECK(c->attackRate().halfAttacks_ == 4);
    CHECK(c->weaponSpeed() == 9);
    CHECK(c->actionType() == ActionType::Cast);
    CHECK(c->faction() == Faction::Purple);
    CHECK(c->hasActed());
}

TEST_CASE("Character::miscActionCost follows the misc action table", "[model][Character]") {
    auto c = makeCharacter("Ragnar", 3);

    const std::vector<std::pair<MiscActionType, int>> costs = {
        {MiscActionType::MiscMagicItem, 3},
        {MiscActionType::Potion,        4},
        {MiscActionType::Ring,          3},
        {MiscActionType::Rod,           1},
        {MiscActionType::Staff,         2},
        {MiscActionType::Wand,          3},
        {MiscActionType::Wait,          0},
        {MiscActionType::Unset,         0},
    };
    for (const auto& [type, cost] : costs) {
        c->setMiscActionType(type);
        INFO("misc action type " << static_cast<int>(type));
        CHECK(c->miscActionCost() == cost);
    }
}

TEST_CASE("Character::miscActionCost for scrolls uses the scroll cost", "[model][Character]") {
    auto c = makeCharacter("Ragnar", 3);
    c->setMiscActionType(MiscActionType::MagicScroll);

    CHECK(c->scrollCost() == -1);
    CHECK(c->miscActionCost() == 0); // unset scroll cost counts as free

    c->setScrollCost(5);
    CHECK(c->miscActionCost() == 5);
}

TEST_CASE("Character combat log and text use the display name", "[model][Character]") {
    auto c = makeCharacter("Ragnar", 3);
    CHECK(c->text() == QString("Ragnar"));
    CHECK(c->combatLog() == QString("Ragnar takes their turn at initiative: 3"));
}

TEST_CASE("Sentinel characters are flagged", "[model][Character]") {
    auto sentinel = Character::createSentinel();
    CHECK(sentinel->isSentinel());

    Character byName(QString("_SENTINEL_"));
    CHECK(byName.isSentinel());

    Character notSentinel(QString("Ragnar"));
    CHECK_FALSE(notSentinel.isSentinel());
}

TEST_CASE("Faction cycling wraps in both directions", "[model][Enums]") {
    CHECK(nextFaction(Faction::Red) == Faction::Blue);
    CHECK(nextFaction(Faction::Purple) == Faction::Red);
    CHECK(prevFaction(Faction::Blue) == Faction::Red);
    CHECK(prevFaction(Faction::Red) == Faction::Purple);
}
