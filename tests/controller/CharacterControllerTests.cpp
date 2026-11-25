#include "TestHelpers.h"

#include <filesystem>

#include "CharacterController.h"
#include "CharacterModel.h"
#include "RosterModel.h"
#include "InitiativeView.h"
#include "AttackAction.h"
#include "CastAction.h"
#include "MiscAction.h"
#include "RemovalEntry.h"
#include "TomlHelper.h"

using namespace Cyrus;
using TestHelpers::makeCharacter;
using TestHelpers::ScopedTempWorkingDir;

namespace {

// The controller needs an InitiativeView for its active-row bookkeeping. It is
// only used as a collaborator here; the view itself is not under test.
struct ControllerFixture {
    InitiativeView view;
    CharacterModel initiative;
    CharacterModel combatLog;
    RosterModel roster;
    CharacterController controller{&view, &initiative, &combatLog, &roster};

    QModelIndex row(int r) const { return initiative.index(r, 0); }

    // addToRoster returns the bottom (sentinel) row so the UI can keep the inline
    // add editor there; this returns the new member's own row instead.
    QModelIndex addRosterMember(const QString& name) {
        controller.addToRoster(name);
        return roster.index(roster.getItems().size() - 1, 0);
    }

    // Puts characters straight into the initiative order and marks the first active.
    void startCombat(const QVector<std::shared_ptr<Character>>& characters) {
        initiative.addItems(characters);
        view.setActiveIndex(row(0));
        initiative.advanceInitiative(characters.first()->initiative());
    }

    std::shared_ptr<Character> active() const {
        return initiative.getItems().value(view.activeIndex().row());
    }

    template <typename T>
    int countOf() const {
        int n = 0;
        for (const auto& c : initiative.getItems())
            if (std::dynamic_pointer_cast<T>(c)) ++n;
        return n;
    }

    QStringList initiativeTexts() const {
        QStringList texts;
        for (const auto& c : initiative.getItems()) texts << c->text();
        return texts;
    }
};

}

// ---- Combat sequence ----

TEST_CASE_METHOD(ControllerFixture, "Controller starts in NPC determination", "[controller][sequence]") {
    CHECK(controller.combatSequenceState() == CombatSequenceState::NPC_DETERMINATION);
    CHECK_FALSE(controller.isGroupInitiative());
    CHECK_FALSE(controller.isRosterModified());
    CHECK(controller.currentRosterName() == "Unset");
}

TEST_CASE_METHOD(ControllerFixture, "Controller cycles through the combat sequence", "[controller][sequence]") {
    QList<CombatSequenceState> emitted;
    QObject::connect(&controller, &CharacterController::combatSequenceStateChanged,
                     [&](CombatSequenceState s) { emitted << s; });

    SECTION("forward wraps from resolution to NPC determination") {
        for (int i = 0; i < 4; ++i) controller.nextCombatSequenceState();
        CHECK(emitted == QList<CombatSequenceState>{
            CombatSequenceState::PLAYER_DETERMINATION,
            CombatSequenceState::INITIATIVE,
            CombatSequenceState::RESOLUTION,
            CombatSequenceState::NPC_DETERMINATION});
    }
    SECTION("backward wraps from NPC determination to resolution") {
        controller.previousCombatSequenceState();
        controller.previousCombatSequenceState();
        CHECK(emitted == QList<CombatSequenceState>{
            CombatSequenceState::RESOLUTION,
            CombatSequenceState::INITIATIVE});
    }
    CHECK(controller.combatSequenceState() == emitted.last());
}

TEST_CASE_METHOD(ControllerFixture, "Changing the combat sequence re-sorts the initiative order", "[controller][sequence]") {
    auto orc = makeCharacter("Orc", 2, Faction::Red);
    auto ragnar = makeCharacter("Ragnar", 5, Faction::Blue);
    initiative.addItems({ragnar, orc});

    controller.nextCombatSequenceState(); // PLAYER_DETERMINATION: blue first
    CHECK(initiative.getItems() == QVector<std::shared_ptr<Character>>{ragnar, orc});

    controller.nextCombatSequenceState(); // INITIATIVE: by initiative
    CHECK(initiative.getItems() == QVector<std::shared_ptr<Character>>{orc, ragnar});
}

TEST_CASE_METHOD(ControllerFixture, "Changing the combat sequence keeps the active character", "[controller][sequence]") {
    auto orc = makeCharacter("Orc", 2, Faction::Red);
    auto ragnar = makeCharacter("Ragnar", 5, Faction::Blue);
    startCombat({orc, ragnar});
    REQUIRE(active() == orc);

    controller.nextCombatSequenceState(); // PLAYER_DETERMINATION moves Ragnar to the top

    CHECK(initiative.getItems() == QVector<std::shared_ptr<Character>>{ragnar, orc});
    CHECK(active() == orc);
}

// ---- Initiative editing ----

TEST_CASE_METHOD(ControllerFixture, "Initiative steps by one within 1..10", "[controller][initiative]") {
    auto ragnar = makeCharacter("Ragnar", 5);
    initiative.appendItem(ragnar);

    controller.incrementInitiative(row(0));
    CHECK(ragnar->initiative() == 6);
    controller.decrementInitiative(row(0));
    controller.decrementInitiative(row(0));
    CHECK(ragnar->initiative() == 4);

    ragnar->setInitiative(10);
    controller.incrementInitiative(row(0));
    CHECK(ragnar->initiative() == 10);

    ragnar->setInitiative(1);
    controller.decrementInitiative(row(0));
    CHECK(ragnar->initiative() == 1);
}

TEST_CASE_METHOD(ControllerFixture, "Initiative edits only touch one character without group initiative", "[controller][initiative]") {
    auto ragnar = makeCharacter("Ragnar", 5, Faction::Blue);
    auto mouse = makeCharacter("Mouse", 5, Faction::Blue);
    initiative.addItems({ragnar, mouse});

    controller.incrementInitiative(row(0));

    CHECK(ragnar->initiative() == 6);
    CHECK(mouse->initiative() == 5);
}

TEST_CASE_METHOD(ControllerFixture, "Group initiative sets the whole faction", "[controller][initiative]") {
    // README: group initiative is per faction, so PCs, town guard and orcs each
    // get their own shared initiative.
    auto ragnar = makeCharacter("Ragnar", 5, Faction::Blue);
    auto mouse = makeCharacter("Mouse", 2, Faction::Blue);
    auto guard = makeCharacter("Guard", 7, Faction::Gray);
    auto orc = makeCharacter("Orc", 9, Faction::Red);
    initiative.addItems({ragnar, mouse, guard, orc});
    controller.setGroupInitiative(true);
    REQUIRE(controller.isGroupInitiative());

    controller.incrementInitiative(row(0));
    CHECK(ragnar->initiative() == 6);
    CHECK(mouse->initiative() == 6);
    CHECK(guard->initiative() == 7);
    CHECK(orc->initiative() == 9);

    controller.decrementInitiative(row(2));
    CHECK(guard->initiative() == 6);
    CHECK(ragnar->initiative() == 6);
    CHECK(orc->initiative() == 9);
}

TEST_CASE_METHOD(ControllerFixture, "Group initiative uses factions changed in the roster mid-combat", "[controller][initiative]") {
    auto ragnar = makeCharacter("Ragnar", 5, Faction::Blue);
    auto guard = makeCharacter("Guard", 7, Faction::Gray);
    roster.addItems({ragnar, guard});
    controller.addRosterToInitiative();
    controller.setGroupInitiative(true);

    // The town guard joins the party's side after combat started.
    controller.nextFaction(roster.index(1, 0)); // Gray -> Orange
    controller.previousFaction(roster.index(1, 0));
    controller.previousFaction(roster.index(1, 0)); // -> Blue
    REQUIRE(guard->faction() == Faction::Blue);

    controller.incrementInitiative(initiative.indexFromCharacter(ragnar));

    CHECK(ragnar->initiative() == 6);
    CHECK(guard->initiative() == 6);
}

TEST_CASE_METHOD(ControllerFixture, "Initiative edits on an invalid row are ignored", "[controller][initiative]") {
    controller.incrementInitiative(QModelIndex());
    controller.decrementInitiative(row(3));
    CHECK(initiative.rowCount() == 0);
}

// ---- Action selection ----

TEST_CASE_METHOD(ControllerFixture, "Selecting an action toggles it", "[controller][actions]") {
    auto ragnar = makeCharacter("Ragnar", 5);
    initiative.appendItem(ragnar);

    controller.updateAction(row(0), ActionType::Attack);
    CHECK(ragnar->actionType() == ActionType::Attack);
    controller.updateAction(row(0), ActionType::Cast);
    CHECK(ragnar->actionType() == ActionType::Cast);
    controller.updateAction(row(0), ActionType::Cast);
    CHECK(ragnar->actionType() == ActionType::Unset);
}

TEST_CASE_METHOD(ControllerFixture, "Selecting a misc action toggles it", "[controller][actions]") {
    auto mouse = makeCharacter("Mouse", 4);
    initiative.appendItem(mouse);

    controller.updateAction(row(0), MiscActionType::Potion);
    CHECK(mouse->miscActionType() == MiscActionType::Potion);
    controller.updateAction(row(0), MiscActionType::Wand);
    CHECK(mouse->miscActionType() == MiscActionType::Wand);
    controller.updateAction(row(0), MiscActionType::Wand);
    CHECK(mouse->miscActionType() == MiscActionType::Unset);
}

// ---- Casting ----

TEST_CASE_METHOD(ControllerFixture, "Cast state starts at casting time 1, duration 0", "[controller][cast]") {
    const QUuid id = QUuid::createUuid();
    CastState state = controller.stateFor(id);
    CHECK(state.castingTime == 1);
    CHECK(state.duration == 0);
    CHECK(state.spellName.isEmpty());
}

TEST_CASE_METHOD(ControllerFixture, "Cast state editing clamps casting time and duration", "[controller][cast]") {
    const QUuid id = QUuid::createUuid();

    controller.setSpellName(id, "Fireball");
    controller.incrementCastingTime(id);
    controller.incrementCastingTime(id);
    controller.incrementDuration(id);
    CHECK(controller.stateFor(id).spellName == QString("Fireball"));
    CHECK(controller.stateFor(id).castingTime == 3);
    CHECK(controller.stateFor(id).duration == 1);

    for (int i = 0; i < 20; ++i) controller.incrementCastingTime(id);
    for (int i = 0; i < 120; ++i) controller.incrementDuration(id);
    CHECK(controller.stateFor(id).castingTime == 10);
    CHECK(controller.stateFor(id).duration == 99);

    for (int i = 0; i < 20; ++i) controller.decrementCastingTime(id);
    for (int i = 0; i < 120; ++i) controller.decrementDuration(id);
    CHECK(controller.stateFor(id).castingTime == 0);
    CHECK(controller.stateFor(id).duration == 0);
}

TEST_CASE_METHOD(ControllerFixture, "Submitting a cast adds a spell to the initiative order", "[controller][cast]") {
    auto wizard = makeCharacter("Elminster", 3);
    initiative.appendItem(wizard);
    const QUuid id = wizard->uuid();
    controller.setSpellName(id, "Shield");
    controller.incrementCastingTime(id); // 2 segments
    for (int i = 0; i < 10; ++i) controller.incrementDuration(id);

    controller.castSubmitted(id, wizard);

    REQUIRE(initiative.rowCount() == 2);
    auto spell = std::dynamic_pointer_cast<CastAction>(initiative.getItems().last());
    REQUIRE(spell);
    CHECK(spell->spellName() == QString("Shield"));
    CHECK(spell->initiative() == 5);
    CHECK(spell->duration() == 10);
    CHECK(spell->uuid() == wizard->uuid());
    CHECK(wizard->hasActed());

    // The pending cast state is consumed.
    CHECK(controller.stateFor(id).spellName.isEmpty());
}

TEST_CASE_METHOD(ControllerFixture, "Submitting a cast without state or character does nothing", "[controller][cast]") {
    auto wizard = makeCharacter("Elminster", 3);
    initiative.appendItem(wizard);

    controller.castSubmitted(wizard->uuid(), wizard); // no cast state yet
    controller.setSpellName(wizard->uuid(), "Shield");
    controller.castSubmitted(wizard->uuid(), nullptr);

    CHECK(initiative.rowCount() == 1);
    CHECK_FALSE(wizard->hasActed());
}

// ---- Attacking ----

TEST_CASE_METHOD(ControllerFixture, "Attack amount and weapon speed steppers", "[controller][attack]") {
    auto ragnar = makeCharacter("Ragnar", 3, Faction::Blue, 2, 4);
    initiative.appendItem(ragnar);

    controller.incrementAttackAmount(row(0));
    CHECK(ragnar->attackRate().toString() == QString("3/2"));
    controller.decrementAttackAmount(row(0));
    controller.decrementAttackAmount(row(0));
    CHECK(ragnar->attackRate().toString() == QString("1"));

    controller.incrementWeaponSpeed(row(0));
    CHECK(ragnar->weaponSpeed() == 5);
    controller.decrementWeaponSpeed(row(0));
    controller.decrementWeaponSpeed(row(0));
    CHECK(ragnar->weaponSpeed() == 3);
}

TEST_CASE_METHOD(ControllerFixture, "Submitting an attack adds one card per attack", "[controller][attack]") {
    // README example: Ragnar, initiative 3, weapon speed 4 -> attacks at 7 and 11.
    auto ragnar = makeCharacter("Ragnar", 3, Faction::Blue, 4, 4);
    initiative.appendItem(ragnar);

    controller.attackSubmitted(row(0));

    REQUIRE(initiative.rowCount() == 3);
    CHECK(ragnar->hasActed());
    auto first = std::dynamic_pointer_cast<AttackAction>(initiative.getItems()[1]);
    auto second = std::dynamic_pointer_cast<AttackAction>(initiative.getItems()[2]);
    REQUIRE(first);
    REQUIRE(second);
    CHECK(first->attackNumber() == 1);
    CHECK(first->initiative() == 7);
    CHECK(second->attackNumber() == 2);
    CHECK(second->initiative() == 11);
}

TEST_CASE_METHOD(ControllerFixture, "Half attack rates alternate attack cards across rounds", "[controller][attack]") {
    // Regression: attackSubmitted used to call attackRate.attack() in the loop
    // condition, on a copy, so half attack rates never alternated.
    // README: a 5/2 fighter puts 2 attack cards in on the first round he attacks,
    // then 3 on the next, and so on. 3/2 is 1, then 2.
    auto [halfAttacks, firstRound, secondRound] = GENERATE(table<int, int, int>({
        {3, 1, 2},
        {5, 2, 3},
    }));
    auto fighter = makeCharacter("Ragnar", 3, Faction::Blue, halfAttacks, 4);
    auto orc = makeCharacter("Orc", 10, Faction::Red);
    // Attacks resolve in the resolution phase, which orders purely by initiative.
    controller.previousCombatSequenceState();
    REQUIRE(controller.combatSequenceState() == CombatSequenceState::RESOLUTION);
    startCombat({fighter, orc});
    INFO("attack rate " << fighter->attackRate().toString().toStdString());

    int round = 1;
    QObject::connect(&controller, &CharacterController::roundChangedText,
                     [&](const QString&) { ++round; });

    auto fighterRow = [&] {
        return row(initiative.getItems().indexOf(fighter));
    };

    // Round 1: attack, then play the round out until it wraps.
    controller.attackSubmitted(fighterRow());
    CHECK(countOf<AttackAction>() == firstRound);
    for (int i = 0; round == 1 && i < 20; ++i) controller.advanceInitiative();
    REQUIRE(round == 2);
    REQUIRE(countOf<AttackAction>() == 0); // every attack card resolved
    REQUIRE_FALSE(fighter->hasActed());    // new round reset the acted flag

    // Round 2: attack again.
    controller.attackSubmitted(fighterRow());
    CHECK(countOf<AttackAction>() == secondRound);
}

TEST_CASE_METHOD(ControllerFixture, "A 5/2 fighter alternates 2 and 3 attacks over four rounds", "[controller][attack]") {
    auto fighter = makeCharacter("Ragnar", 3, Faction::Blue, 5, 4);
    initiative.appendItem(fighter);

    QList<int> perRound;
    for (int round = 0; round < 4; ++round) {
        controller.attackSubmitted(row(0));
        perRound << countOf<AttackAction>();
        initiative.removeRows(1, initiative.rowCount() - 1); // attacks resolved
    }

    CHECK(perRound == QList<int>{2, 3, 2, 3});
}

TEST_CASE_METHOD(ControllerFixture, "Cancelling an attack gives back the half attack", "[controller][attack]") {
    auto fighter = makeCharacter("Ragnar", 3, Faction::Blue, 5, 4);
    initiative.appendItem(fighter);

    controller.attackSubmitted(row(0));
    REQUIRE(countOf<AttackAction>() == 2);
    controller.cancelAction(row(0));
    REQUIRE(countOf<AttackAction>() == 0);

    // Resubmitting the same round gives the same count, and the next round still
    // gets the extra attack.
    controller.attackSubmitted(row(0));
    CHECK(countOf<AttackAction>() == 2);
    initiative.removeRows(1, initiative.rowCount() - 1);
    controller.attackSubmitted(row(0));
    CHECK(countOf<AttackAction>() == 3);
}

TEST_CASE_METHOD(ControllerFixture, "Cancelling a cast does not touch the attack rate", "[controller][attack]") {
    auto wizard = makeCharacter("Elminster", 3, Faction::Blue, 5, 4);
    initiative.appendItem(wizard);
    controller.setSpellName(wizard->uuid(), "Shield");
    controller.castSubmitted(wizard->uuid(), wizard);

    controller.cancelAction(row(0));

    // Still on the first (2 attack) half of the 5/2 cycle.
    controller.attackSubmitted(row(0));
    CHECK(countOf<AttackAction>() == 2);
}

TEST_CASE_METHOD(ControllerFixture, "Submitting an attack on an invalid row does nothing", "[controller][attack]") {
    controller.attackSubmitted(QModelIndex());
    CHECK(initiative.rowCount() == 0);
}

// ---- Misc actions ----

TEST_CASE_METHOD(ControllerFixture, "Submitting a misc action adds it at initiative + cost", "[controller][misc]") {
    auto mouse = makeCharacter("Mouse", 4);
    mouse->setMiscActionType(MiscActionType::Potion);
    initiative.appendItem(mouse);

    controller.miscSubmitted(row(0));

    REQUIRE(initiative.rowCount() == 2);
    auto potion = std::dynamic_pointer_cast<MiscAction>(initiative.getItems()[1]);
    REQUIRE(potion);
    CHECK(potion->initiative() == 8);
    CHECK(potion->text() == QString("Mouse drinks a Potion"));
    CHECK(mouse->hasActed());

    controller.miscSubmitted(QModelIndex());
    CHECK(initiative.rowCount() == 2);
}

// ---- Advancing the initiative order ----

TEST_CASE_METHOD(ControllerFixture, "Advancing initiative logs the turn and moves to the next row", "[controller][advance]") {
    auto orc = makeCharacter("Orc", 2, Faction::Red);
    auto ragnar = makeCharacter("Ragnar", 5, Faction::Blue);
    startCombat({orc, ragnar});

    QList<int> changedRows;
    QObject::connect(&controller, &CharacterController::currentInitiativeChanged,
                     [&](const QModelIndex& idx) { changedRows << idx.row(); });

    controller.advanceInitiative();

    CHECK(active() == ragnar);
    CHECK(combatLog.getItems() == QVector<std::shared_ptr<Character>>{orc});
    CHECK(initiative.data(row(0), InitiativeRole).toInt() == 5);
    CHECK(changedRows == QList<int>{1});
}

TEST_CASE_METHOD(ControllerFixture, "Advancing past the last row wraps into the next round", "[controller][advance]") {
    auto orc = makeCharacter("Orc", 2, Faction::Red);
    auto ragnar = makeCharacter("Ragnar", 5, Faction::Blue);
    startCombat({orc, ragnar});

    QStringList roundTexts;
    QObject::connect(&controller, &CharacterController::roundChangedText,
                     [&](const QString& t) { roundTexts << t; });

    controller.advanceInitiative();
    controller.advanceInitiative();

    CHECK(active() == orc);
    CHECK(roundTexts == QStringList{"Round: 2"});
    CHECK(combatLog.rowCount() == 2);
}

TEST_CASE_METHOD(ControllerFixture, "Attack cards are removed once they resolve", "[controller][advance]") {
    auto ragnar = makeCharacter("Ragnar", 3, Faction::Blue, 2, 4);
    auto orc = makeCharacter("Orc", 9, Faction::Red);
    auto attack = std::make_shared<AttackAction>(*ragnar, 1); // initiative 7
    startCombat({ragnar, attack, orc});

    controller.advanceInitiative(); // Ragnar's turn
    REQUIRE(active() == attack);
    controller.advanceInitiative(); // attack resolves

    CHECK(countOf<AttackAction>() == 0);
    CHECK(active() == orc);
    CHECK(combatLog.getItems().last() == attack);
}

TEST_CASE_METHOD(ControllerFixture, "Misc action cards are removed once they resolve", "[controller][advance]") {
    auto mouse = makeCharacter("Mouse", 4);
    mouse->setMiscActionType(MiscActionType::Rod);
    auto rod = std::make_shared<MiscAction>(*mouse);
    auto orc = makeCharacter("Orc", 9);
    startCombat({rod, orc});

    controller.advanceInitiative();

    CHECK(countOf<MiscAction>() == 0);
    CHECK(active() == orc);
}

TEST_CASE_METHOD(ControllerFixture, "Spells stay in the order until their duration runs out", "[controller][advance]") {
    auto wizard = makeCharacter("Elminster", 3);
    auto spell = std::make_shared<CastAction>(*wizard, "Shield", 2, 2);
    auto orc = makeCharacter("Orc", 9);
    startCombat({spell, orc});

    // Cast resolves: the spell stays and starts counting down.
    controller.advanceInitiative();
    CHECK(countOf<CastAction>() == 1);
    CHECK(spell->duration() == 1);
    CHECK(spell->text() == QString("Elminster has cast Shield"));

    controller.advanceInitiative(); // orc, wraps to the spell
    controller.advanceInitiative(); // round 2 tick
    CHECK(countOf<CastAction>() == 1);
    CHECK(spell->duration() == 0);

    controller.advanceInitiative(); // orc
    controller.advanceInitiative(); // expired: removed
    CHECK(countOf<CastAction>() == 0);
    CHECK(initiative.getItems() == QVector<std::shared_ptr<Character>>{orc});
}

TEST_CASE_METHOD(ControllerFixture, "Instant spells are removed when they resolve", "[controller][advance]") {
    auto wizard = makeCharacter("Elminster", 3);
    auto fireball = std::make_shared<CastAction>(*wizard, "Fireball", 3, 0);
    auto orc = makeCharacter("Orc", 9);
    startCombat({fireball, orc});

    controller.advanceInitiative();

    CHECK(countOf<CastAction>() == 0);
    CHECK(active() == orc);
}

TEST_CASE_METHOD(ControllerFixture, "Advancing without an active row does nothing", "[controller][advance]") {
    initiative.appendItem(makeCharacter("Orc", 2));

    controller.advanceInitiative();
    controller.backtrackInitiative();

    CHECK(combatLog.rowCount() == 0);
    CHECK_FALSE(view.hasActiveIndex());
}

TEST_CASE_METHOD(ControllerFixture, "Backtracking moves to the previous row and wraps", "[controller][advance]") {
    auto orc = makeCharacter("Orc", 2);
    auto guard = makeCharacter("Guard", 4);
    auto ragnar = makeCharacter("Ragnar", 5);
    startCombat({orc, guard, ragnar});
    controller.advanceInitiative();
    REQUIRE(active() == guard);

    controller.backtrackInitiative();
    CHECK(active() == orc);

    controller.backtrackInitiative();
    CHECK(active() == ragnar);
    CHECK(initiative.data(row(0), InitiativeRole).toInt() == 5);
}

// ---- Removing from the initiative order ----

TEST_CASE_METHOD(ControllerFixture, "Cancelling an action removes its pending cards", "[controller][delete]") {
    auto ragnar = makeCharacter("Ragnar", 3, Faction::Blue, 4, 4);
    auto orc = makeCharacter("Orc", 9);
    initiative.addItems({ragnar, orc});
    controller.attackSubmitted(row(0));
    REQUIRE(countOf<AttackAction>() == 2);

    controller.cancelAction(row(0));

    CHECK(countOf<AttackAction>() == 0);
    CHECK_FALSE(ragnar->hasActed());
    CHECK(initiative.getItems() == QVector<std::shared_ptr<Character>>{ragnar, orc});
}

TEST_CASE_METHOD(ControllerFixture, "Deleting a character also removes its pending cards", "[controller][delete]") {
    auto ragnar = makeCharacter("Ragnar", 3, Faction::Blue, 4, 4);
    auto orc = makeCharacter("Orc", 9);
    initiative.addItems({ragnar, orc});
    controller.attackSubmitted(row(0));

    controller.deleteItem(row(0));

    CHECK(initiative.getItems() == QVector<std::shared_ptr<Character>>{orc});
}

TEST_CASE_METHOD(ControllerFixture, "Deleting the active character hands the turn to the next row", "[controller][delete]") {
    auto orc = makeCharacter("Orc", 2);
    auto ragnar = makeCharacter("Ragnar", 5);
    startCombat({orc, ragnar});

    QList<int> changedRows;
    QObject::connect(&controller, &CharacterController::currentInitiativeChanged,
                     [&](const QModelIndex& idx) { changedRows << idx.row(); });

    controller.deleteItem(row(0));

    CHECK(initiative.getItems() == QVector<std::shared_ptr<Character>>{ragnar});
    CHECK(active() == ragnar);
    CHECK(initiative.data(row(0), InitiativeRole).toInt() == 5);
    CHECK_FALSE(changedRows.isEmpty());
    // The deleted creature never acted: only its removal is logged, not a turn.
    REQUIRE(combatLog.rowCount() == 1);
    CHECK(combatLog.getItems().first()->combatLog() == QString("Orc was removed from combat."));
}

TEST_CASE_METHOD(ControllerFixture, "Deleting the last active row wraps the turn to the top", "[controller][delete]") {
    auto orc = makeCharacter("Orc", 2);
    auto ragnar = makeCharacter("Ragnar", 5);
    startCombat({orc, ragnar});
    controller.advanceInitiative();
    REQUIRE(active() == ragnar);

    QStringList roundTexts;
    QObject::connect(&controller, &CharacterController::roundChangedText,
                     [&](const QString& t) { roundTexts << t; });

    controller.deleteItem(row(1));

    CHECK(active() == orc);
    CHECK(roundTexts == QStringList{"Round: 2"});
}

TEST_CASE_METHOD(ControllerFixture, "Deleting the only row leaves no active row", "[controller][delete]") {
    startCombat({makeCharacter("Orc", 2)});

    controller.deleteItem(row(0));

    CHECK(initiative.rowCount() == 0);
    CHECK_FALSE(view.hasActiveIndex());
}

TEST_CASE_METHOD(ControllerFixture, "Deleting the active character skips past its own cards", "[controller][delete]") {
    auto ragnar = makeCharacter("Ragnar", 3, Faction::Blue, 4, 4);
    auto orc = makeCharacter("Orc", 9);
    controller.previousCombatSequenceState(); // RESOLUTION: order by initiative only
    startCombat({ragnar, orc});
    controller.attackSubmitted(row(0)); // attack 1 (initiative 7) lands right after Ragnar
    REQUIRE(initiative.getItems()[1]->uuid() == ragnar->uuid());

    controller.deleteItem(row(0));

    CHECK(initiative.getItems() == QVector<std::shared_ptr<Character>>{orc});
    CHECK(active() == orc);
}

TEST_CASE_METHOD(ControllerFixture, "Deleting a character whose active card wraps onto another of its cards", "[controller][delete]") {
    // Rows: [Ragnar's attack 2] [Orc] [Ragnar] [Ragnar's attack 1 (active)].
    // Handing the turn on from the last row wraps to row 0, which is also being removed.
    auto ragnar = makeCharacter("Ragnar", 3, Faction::Blue, 4, 0);
    auto orc = makeCharacter("Orc", 9);
    auto attack1 = std::make_shared<AttackAction>(*ragnar, 1);
    auto attack2 = std::make_shared<AttackAction>(*ragnar, 2);
    initiative.addItems({attack2, orc, ragnar, attack1});
    view.setActiveIndex(row(3));

    controller.deleteItem(row(2));

    CHECK(initiative.getItems() == QVector<std::shared_ptr<Character>>{orc});
    CHECK(active() == orc);
}

TEST_CASE_METHOD(ControllerFixture, "Deleting the active spell or misc card removes only that card", "[controller][delete]") {
    auto wizard = makeCharacter("Elminster", 3);
    auto orc = makeCharacter("Orc", 9);
    std::shared_ptr<Character> card;
    SECTION("spell") {
        card = std::make_shared<CastAction>(*wizard, "Shield", 2, 10);
    }
    SECTION("misc") {
        wizard->setMiscActionType(MiscActionType::Potion);
        card = std::make_shared<MiscAction>(*wizard);
    }
    startCombat({card, orc, wizard});

    controller.deleteItem(row(0));

    CHECK(initiative.getItems() == QVector<std::shared_ptr<Character>>{orc, wizard});
    CHECK(active() == orc);
    CHECK(combatLog.rowCount() == 0);
}

TEST_CASE_METHOD(ControllerFixture, "Cancelling removes cards that sort above their character", "[controller][delete]") {
    // Weapon speed 0 puts the attack card at Ragnar's own initiative, so a sort
    // can place it above him.
    auto ragnar = makeCharacter("Ragnar", 5, Faction::Blue, 2, 0);
    auto orc = makeCharacter("Orc", 9);
    auto attack = std::make_shared<AttackAction>(*ragnar, 1);
    initiative.addItems({attack, ragnar, orc});

    controller.cancelAction(row(1));

    CHECK(initiative.getItems() == QVector<std::shared_ptr<Character>>{ragnar, orc});
}

TEST_CASE_METHOD(ControllerFixture, "Cancelling on an action card does nothing", "[controller][delete]") {
    // Only base characters have a cancel button.
    auto ragnar = makeCharacter("Ragnar", 3, Faction::Blue, 2, 4);
    auto attack = std::make_shared<AttackAction>(*ragnar, 1);
    initiative.addItems({ragnar, attack});
    ragnar->setActed(true);

    controller.cancelAction(row(1));

    CHECK(initiative.rowCount() == 2);
    CHECK(ragnar->hasActed());
}

TEST_CASE_METHOD(ControllerFixture, "Deleting the active attack card removes only that card", "[controller][delete]") {
    // Regression: deleteItem used to advance first, which already removed the
    // attack card, then removed index.row() again, deleting the next character.
    auto ragnar = makeCharacter("Ragnar", 3, Faction::Blue, 2, 4);
    auto attack = std::make_shared<AttackAction>(*ragnar, 1); // initiative 7
    auto orc = makeCharacter("Orc", 9);
    auto guard = makeCharacter("Guard", 10);
    startCombat({attack, orc, guard});

    controller.deleteItem(row(0));

    CHECK(initiative.getItems() == QVector<std::shared_ptr<Character>>{orc, guard});
}

TEST_CASE_METHOD(ControllerFixture, "Deleting a creature logs its removal", "[controller][delete]") {
    auto orc = makeCharacter("Orc", 2, Faction::Red);
    auto ragnar = makeCharacter("Ragnar", 5, Faction::Blue);
    startCombat({orc, ragnar});
    combatLog.appendItem(orc); // an earlier turn

    int scrollRequests = 0;
    QObject::connect(&controller, &CharacterController::requestScrollCombatLogToBottom,
                     [&] { ++scrollRequests; });

    controller.deleteItem(row(1));

    REQUIRE(combatLog.rowCount() == 2);
    auto entry = std::dynamic_pointer_cast<RemovalEntry>(combatLog.getItems().last());
    REQUIRE(entry);
    CHECK(entry->combatLog() == QString("Ragnar was removed from combat."));
    CHECK(entry->uuid() == ragnar->uuid());
    CHECK(entry->characterType() == ragnar->characterType()); // log shows the class icon
    CHECK(scrollRequests == 1);
}

TEST_CASE_METHOD(ControllerFixture, "Deleting a numbered creature logs its numbered name", "[controller][delete]") {
    auto goblin1 = makeCharacter("Goblin", 2);
    auto goblin2 = makeCharacter("Goblin", 4);
    startCombat({goblin1, goblin2});

    controller.deleteItem(row(1));

    REQUIRE(combatLog.rowCount() == 1);
    CHECK(combatLog.getItems().first()->combatLog() == QString("Goblin 2 was removed from combat."));
}

TEST_CASE_METHOD(ControllerFixture, "Deleting a creature with pending cards logs one removal", "[controller][delete]") {
    auto ragnar = makeCharacter("Ragnar", 3, Faction::Blue, 4, 4);
    auto orc = makeCharacter("Orc", 9);
    initiative.addItems({ragnar, orc});
    controller.attackSubmitted(row(0));
    controller.setSpellName(ragnar->uuid(), "Shield");
    controller.castSubmitted(ragnar->uuid(), ragnar);
    REQUIRE(initiative.rowCount() == 5);

    controller.deleteItem(initiative.indexFromCharacter(ragnar));

    REQUIRE(combatLog.rowCount() == 1);
    CHECK(combatLog.getItems().first()->combatLog() == QString("Ragnar was removed from combat."));
}

TEST_CASE_METHOD(ControllerFixture, "Deleting an action card does not log a removal", "[controller][delete]") {
    auto ragnar = makeCharacter("Ragnar", 3, Faction::Blue, 2, 4);
    auto orc = makeCharacter("Orc", 9);
    initiative.addItems({ragnar, orc});
    controller.attackSubmitted(row(0));
    REQUIRE(initiative.rowCount() == 3);

    int scrollRequests = 0;
    QObject::connect(&controller, &CharacterController::requestScrollCombatLogToBottom,
                     [&] { ++scrollRequests; });

    controller.deleteItem(row(initiative.rowCount() - 1)); // the attack card

    CHECK(initiative.rowCount() == 2);
    CHECK(combatLog.rowCount() == 0);
    CHECK(scrollRequests == 0);
}

TEST_CASE_METHOD(ControllerFixture, "Deleting nothing logs nothing", "[controller][delete]") {
    initiative.appendItem(makeCharacter("Orc", 2));

    controller.deleteItem(QModelIndex());
    controller.deleteItem(row(5));

    CHECK(initiative.rowCount() == 1);
    CHECK(combatLog.rowCount() == 0);
}

TEST_CASE_METHOD(ControllerFixture, "Clearing the initiative order resets the round", "[controller][delete]") {
    startCombat({makeCharacter("Orc", 2)});
    combatLog.appendItem(makeCharacter("Ragnar", 5));

    QStringList roundTexts;
    QObject::connect(&controller, &CharacterController::roundChangedText,
                     [&](const QString& t) { roundTexts << t; });

    controller.clearInitiativeOrder();
    CHECK(initiative.rowCount() == 0);
    CHECK(roundTexts == QStringList{"Round: 1"});
    CHECK(combatLog.rowCount() == 1); // clearing is not a per-creature removal

    controller.clearCombatLog();
    CHECK(combatLog.rowCount() == 0);
}

TEST_CASE_METHOD(ControllerFixture, "A new combat after clearing starts counting from round 1", "[controller][delete]") {
    // Regression: clearInitiativeOrder used to emit roundUpdate(1) for the label
    // without resetting CharacterModel::round_.
    auto orc = makeCharacter("Orc", 2);
    auto ragnar = makeCharacter("Ragnar", 5);
    startCombat({orc, ragnar});
    controller.advanceInitiative();
    controller.advanceInitiative(); // round 2

    controller.clearInitiativeOrder();
    view.setActiveIndex(QModelIndex());

    QStringList roundTexts;
    QObject::connect(&controller, &CharacterController::roundChangedText,
                     [&](const QString& t) { roundTexts << t; });

    startCombat({orc, ragnar});
    controller.advanceInitiative();
    controller.advanceInitiative(); // wraps once: should be round 2 again

    CHECK(roundTexts.last() == QString("Round: 2"));
}

// ---- Roster ----

TEST_CASE_METHOD(ControllerFixture, "Adding to the roster creates a default character", "[controller][roster]") {
    QModelIndex bottom = controller.addToRoster("Ragnar");

    // The returned index is the sentinel row below the new member.
    CHECK(bottom.row() == 1);
    CHECK(roster.data(bottom, CharacterRole).value<std::shared_ptr<Character>>()->isSentinel());

    REQUIRE(roster.getItems().size() == 1);
    auto ragnar = roster.getItems().first();
    CHECK(ragnar->name() == QString("Ragnar"));
    CHECK(ragnar->initiative() == 0);
    CHECK(ragnar->attackRate().toString() == QString("1"));
    CHECK(ragnar->faction() == Faction::Red);
    CHECK(controller.isRosterModified());
}

TEST_CASE_METHOD(ControllerFixture, "Roster factions cycle forward and back", "[controller][roster]") {
    QModelIndex idx = addRosterMember("Ragnar");

    controller.nextFaction(idx);
    CHECK(roster.getItem(idx)->faction() == Faction::Blue);
    controller.previousFaction(idx);
    controller.previousFaction(idx);
    CHECK(roster.getItem(idx)->faction() == Faction::Purple);
}

TEST_CASE_METHOD(ControllerFixture, "Cloning roster members numbers them", "[controller][roster]") {
    // README example: add a drow fighter, clone 4 times, get 5 numbered fighters.
    QModelIndex idx = addRosterMember("Drow Fighter");
    for (int i = 0; i < 4; ++i) controller.cloneRosterMember(idx);

    QStringList names;
    for (const auto& c : roster.getItems()) names << c->name();
    CHECK(names == QStringList{"Drow Fighter 1", "Drow Fighter 2", "Drow Fighter 3",
                               "Drow Fighter 4", "Drow Fighter 5"});
}

TEST_CASE_METHOD(ControllerFixture, "Deleting a roster member", "[controller][roster]") {
    controller.addToRoster("Ragnar");
    QModelIndex orc = addRosterMember("Orc");

    controller.deleteRosterMember(orc);

    REQUIRE(roster.getItems().size() == 1);
    CHECK(roster.getItems().first()->name() == QString("Ragnar"));
}

TEST_CASE_METHOD(ControllerFixture, "Adding the roster to initiative starts combat", "[controller][roster]") {
    auto ragnar = makeCharacter("Ragnar", 5, Faction::Blue);
    auto orc = makeCharacter("Orc", 7, Faction::Red);
    auto goblin = makeCharacter("Goblin", 3, Faction::Red);
    roster.addItems({ragnar, orc, goblin});

    controller.addRosterToInitiative();

    // NPC determination: non-blue first, then by initiative.
    CHECK(initiative.getItems() == QVector<std::shared_ptr<Character>>{goblin, orc, ragnar});
    CHECK(active() == goblin);
    CHECK(initiative.data(row(0), InitiativeRole).toInt() == 3);
}

TEST_CASE_METHOD(ControllerFixture, "Adding one roster member to initiative", "[controller][roster]") {
    auto ragnar = makeCharacter("Ragnar", 5, Faction::Blue);
    auto orc = makeCharacter("Orc", 7, Faction::Red);
    roster.addItems({ragnar, orc});

    controller.addRosterMemberToInitiative(roster.index(1, 0));
    CHECK(initiative.getItems() == QVector<std::shared_ptr<Character>>{orc});
    CHECK(active() == orc);

    controller.addRosterMemberToInitiative(roster.index(0, 0));
    CHECK(initiative.rowCount() == 2);
    CHECK(active() == orc); // already had an active row

    controller.addRosterMemberToInitiative(roster.index(2, 0)); // sentinel row
    CHECK(initiative.rowCount() == 2);
}

TEST_CASE_METHOD(ControllerFixture, "Saving and loading a roster", "[controller][roster]") {
    ScopedTempWorkingDir tmp;
    QModelIndex ragnarIdx = addRosterMember("Ragnar");
    controller.nextFaction(ragnarIdx);
    roster.getItem(ragnarIdx)->setAttackRate(AttackRate{5});
    controller.addToRoster("Orc");

    controller.saveRoster("party");
    CHECK_FALSE(controller.isRosterModified());
    CHECK(controller.currentRosterName() == "party");
    CHECK(std::filesystem::exists(tmp.path() / "cyrus_data" / "roster" / "party.toml"));

    roster.clear();
    QModelIndex last = controller.loadRoster("party");

    REQUIRE(roster.getItems().size() == 2);
    CHECK(roster.getItems()[0]->name() == QString("Ragnar"));
    CHECK(roster.getItems()[0]->faction() == Faction::Blue);
    CHECK(roster.getItems()[0]->attackRate().toString() == QString("5/2"));
    CHECK(roster.getItems()[1]->name() == QString("Orc"));
    CHECK(roster.getItems()[1]->attackRate().toString() == QString("1"));
    CHECK(last.row() == roster.rowCount() - 1);
    CHECK(controller.currentRosterName() == "party");
}

TEST_CASE_METHOD(ControllerFixture, "Saving an empty roster writes nothing", "[controller][roster]") {
    ScopedTempWorkingDir tmp;
    controller.saveRoster("empty");
    CHECK_FALSE(std::filesystem::exists(tmp.path() / "cyrus_data" / "roster" / "empty.toml"));
    CHECK(controller.currentRosterName() == "Unset");
}

TEST_CASE_METHOD(ControllerFixture, "Reloading a roster does not keep counting up names", "[controller][roster]") {
    ScopedTempWorkingDir tmp;
    QModelIndex goblin = addRosterMember("Goblin");
    controller.cloneRosterMember(goblin);
    controller.saveRoster("goblins");

    controller.loadRoster("goblins");
    controller.loadRoster("goblins");

    QStringList names;
    for (const auto& c : roster.getItems()) names << c->name();
    CHECK(names == QStringList{"Goblin 1", "Goblin 2"});
}
