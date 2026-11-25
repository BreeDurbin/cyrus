#include "TestHelpers.h"

#include "CharacterModel.h"
#include "AttackAction.h"
#include "CastAction.h"

using namespace Cyrus;
using TestHelpers::makeCharacter;

namespace {

QStringList namesInOrder(const CharacterModel& model) {
    QStringList names;
    for (const auto& c : model.getItems()) names << c->name();
    return names;
}

}

TEST_CASE("CharacterModel starts empty", "[model][CharacterModel]") {
    CharacterModel model;
    CHECK(model.rowCount() == 0);
    CHECK(model.getItems().isEmpty());
    CHECK(model.getItem(model.index(0, 0)) == nullptr);
}

TEST_CASE("CharacterModel appends and adds items in order", "[model][CharacterModel]") {
    CharacterModel model;
    auto a = makeCharacter("Ragnar", 3);
    auto b = makeCharacter("Elminster", 5);
    auto c = makeCharacter("Mouse", 1);

    model.appendItem(a);
    model.addItems({b, c});

    REQUIRE(model.rowCount() == 3);
    CHECK(model.getItem(model.index(0, 0)) == a);
    CHECK(model.getItem(model.index(1, 0)) == b);
    CHECK(model.getItem(model.index(2, 0)) == c);
    CHECK(model.indexFromCharacter(c).row() == 2);
}

TEST_CASE("CharacterModel::addItems with nothing is a no-op", "[model][CharacterModel]") {
    CharacterModel model;
    model.addItems({});
    CHECK(model.rowCount() == 0);
}

TEST_CASE("CharacterModel exposes characters through its roles", "[model][CharacterModel]") {
    CharacterModel model;
    auto ragnar = makeCharacter("Ragnar", 3);
    model.appendItem(ragnar);
    const QModelIndex idx = model.index(0, 0);

    CHECK(model.data(idx, Qt::DisplayRole).toString() == QString("Ragnar"));
    CHECK(model.data(idx, CharacterRole).value<std::shared_ptr<Character>>() == ragnar);
    CHECK(model.data(idx, InitiativeRole).toInt() == 0);
    CHECK_FALSE(model.data(model.index(5, 0), Qt::DisplayRole).isValid());
}

TEST_CASE("CharacterModel marks base characters casting a spell as editable", "[model][CharacterModel]") {
    CharacterModel model;
    auto wizard = makeCharacter("Elminster", 3);
    auto fighter = makeCharacter("Ragnar", 3);
    wizard->setActionType(ActionType::Cast);
    fighter->setActionType(ActionType::Attack);
    auto spell = std::make_shared<CastAction>(*wizard, "Shield", 1, 10);
    model.addItems({wizard, fighter, spell});

    CHECK(model.data(model.index(0, 0), RoleTypeRole).toInt() == SpellNameEditRole);
    CHECK(model.data(model.index(1, 0), RoleTypeRole).toInt() == ActionNameEditRole);
    CHECK_FALSE(model.data(model.index(2, 0), RoleTypeRole).isValid());

    CHECK(model.flags(model.index(0, 0)).testFlag(Qt::ItemIsEditable));
    CHECK_FALSE(model.flags(model.index(1, 0)).testFlag(Qt::ItemIsEditable));
    CHECK_FALSE(model.flags(model.index(2, 0)).testFlag(Qt::ItemIsEditable));
    CHECK(model.flags(QModelIndex()) == Qt::NoItemFlags);
}

TEST_CASE("CharacterModel groups items by faction", "[model][CharacterModel]") {
    CharacterModel model;
    auto pc1 = makeCharacter("Ragnar", 3, Faction::Blue);
    auto pc2 = makeCharacter("Elminster", 5, Faction::Blue);
    auto orc = makeCharacter("Orc", 2, Faction::Red);
    model.addItems({pc1, orc, pc2});

    auto blue = model.getItems(Faction::Blue);
    CHECK(blue.size() == 2);
    CHECK(blue.contains(pc1));
    CHECK(blue.contains(pc2));
    CHECK(model.getItems(Faction::Red) == QVector<std::shared_ptr<Character>>{orc});
    CHECK(model.getItems(Faction::Gray).isEmpty());
}

TEST_CASE("CharacterModel removes items and keeps lookups in sync", "[model][CharacterModel]") {
    CharacterModel model;
    auto a = makeCharacter("Ragnar", 3, Faction::Blue);
    auto b = makeCharacter("Orc", 2, Faction::Red);
    auto c = makeCharacter("Mouse", 1, Faction::Blue);
    model.addItems({a, b, c});

    SECTION("removeItem") {
        model.removeItem(model.index(0, 0));
    }
    SECTION("removeRows") {
        CHECK(model.removeRows(0, 1));
    }

    REQUIRE(model.rowCount() == 2);
    CHECK(model.getItems() == QVector<std::shared_ptr<Character>>{b, c});
    CHECK_FALSE(model.indexFromCharacter(a).isValid());
    CHECK(model.indexFromCharacter(b).row() == 0);
    CHECK(model.indexFromCharacter(c).row() == 1);
    CHECK(model.getItems(Faction::Blue) == QVector<std::shared_ptr<Character>>{c});
}

TEST_CASE("CharacterModel::removeRows rejects out of range rows", "[model][CharacterModel]") {
    CharacterModel model;
    model.appendItem(makeCharacter("Ragnar", 3));

    CHECK_FALSE(model.removeRows(-1, 1));
    CHECK_FALSE(model.removeRows(0, 2));
    model.removeItem(QModelIndex());
    CHECK(model.rowCount() == 1);
}

TEST_CASE("CharacterModel::setItem replaces a row", "[model][CharacterModel]") {
    CharacterModel model;
    auto old = makeCharacter("Ragnar", 3, Faction::Blue);
    auto replacement = makeCharacter("Orc", 2, Faction::Red);
    model.appendItem(old);

    model.setItem(model.index(0, 0), replacement);

    CHECK(model.getItem(model.index(0, 0)) == replacement);
    CHECK_FALSE(model.indexFromCharacter(old).isValid());
    CHECK(model.indexFromCharacter(replacement).row() == 0);
    CHECK(model.getItems(Faction::Blue).isEmpty());
    CHECK(model.getItems(Faction::Red) == QVector<std::shared_ptr<Character>>{replacement});
}

TEST_CASE("CharacterModel::clear empties the model", "[model][CharacterModel]") {
    CharacterModel model;
    model.addItems({makeCharacter("Ragnar", 3), makeCharacter("Orc", 2)});
    model.clear();
    CHECK(model.rowCount() == 0);
}

TEST_CASE("CharacterModel::clear resets the round and segment", "[model][CharacterModel]") {
    CharacterModel model;
    model.appendItem(makeCharacter("Ragnar", 3));
    model.advanceInitiative(5);
    model.advanceInitiative(2); // round 2

    QList<int> rounds;
    QObject::connect(&model, &CharacterModel::roundUpdate, [&](int r) { rounds << r; });
    model.clear();
    CHECK(rounds == QList<int>{1});

    model.appendItem(makeCharacter("Orc", 4));
    CHECK(model.data(model.index(0, 0), InitiativeRole).toInt() == 0);
    model.advanceInitiative(4); // first segment of the new combat, still round 1
    CHECK(rounds == QList<int>{1});
}

TEST_CASE("CharacterModel::clear also clears faction lookups", "[model][CharacterModel]") {
    // Regression: clear() used to leave cached row/faction lookups populated, so
    // group initiative still found characters that were cleared.
    CharacterModel model;
    auto pc = makeCharacter("Ragnar", 3, Faction::Blue);
    model.appendItem(pc);

    model.clear();

    CHECK(model.getItems(Faction::Blue).isEmpty());
    CHECK_FALSE(model.indexFromCharacter(pc).isValid());
}

TEST_CASE("CharacterModel sorts by combat sequence state", "[model][CharacterModel]") {
    CharacterModel model;
    auto orc = makeCharacter("Orc", 2, Faction::Red);
    auto ragnar = makeCharacter("Ragnar", 5, Faction::Blue);
    auto guard = makeCharacter("Guard", 1, Faction::Gray);
    auto mouse = makeCharacter("Mouse", 3, Faction::Blue);
    model.addItems({orc, ragnar, guard, mouse});

    SECTION("RESOLUTION: initiative only") {
        model.sort(0, CombatSequenceState::RESOLUTION);
        CHECK(namesInOrder(model) == QStringList{"Guard", "Orc", "Mouse", "Ragnar"});
    }
    SECTION("PLAYER_DETERMINATION: blue faction first") {
        model.sort(0, CombatSequenceState::PLAYER_DETERMINATION);
        CHECK(namesInOrder(model) == QStringList{"Mouse", "Ragnar", "Guard", "Orc"});
    }
    SECTION("NPC_DETERMINATION: non-blue factions first") {
        model.sort(0, CombatSequenceState::NPC_DETERMINATION);
        CHECK(namesInOrder(model) == QStringList{"Guard", "Orc", "Mouse", "Ragnar"});
    }
}

TEST_CASE("CharacterModel INITIATIVE sort puts characters before their actions", "[model][CharacterModel]") {
    CharacterModel model;
    auto ragnar = makeCharacter("Ragnar", 5, Faction::Blue, 2, 4);
    auto orc = makeCharacter("Orc", 8, Faction::Red);
    auto attack = std::make_shared<AttackAction>(*ragnar, 1); // initiative 9
    auto spell = std::make_shared<CastAction>(*orc, "Darkness", 0, 3); // initiative 8
    model.addItems({attack, orc, spell, ragnar});

    model.sort(0, CombatSequenceState::INITIATIVE);

    auto items = model.getItems();
    CHECK(items[0] == ragnar);
    CHECK(items[1] == orc);
    CHECK(items[2] == spell);
    CHECK(items[3] == attack);
}

TEST_CASE("CharacterModel::sort only reorders from the given row", "[model][CharacterModel]") {
    CharacterModel model;
    model.addItems({makeCharacter("C", 9), makeCharacter("B", 6), makeCharacter("A", 2)});

    model.sort(1, CombatSequenceState::RESOLUTION);
    CHECK(namesInOrder(model) == QStringList{"C", "A", "B"});

    model.sort(5, CombatSequenceState::RESOLUTION); // out of range: no-op
    CHECK(namesInOrder(model) == QStringList{"C", "A", "B"});
}

TEST_CASE("CharacterModel::indexFromCharacter is correct after sorting", "[model][CharacterModel]") {
    // Regression: sort() used to leave a cached row lookup pointing at old rows.
    CharacterModel model;
    auto slow = makeCharacter("Slow", 9);
    auto fast = makeCharacter("Fast", 1);
    model.addItems({slow, fast});

    model.sort(0, CombatSequenceState::RESOLUTION);

    CHECK(model.indexFromCharacter(fast).row() == 0);
    CHECK(model.indexFromCharacter(slow).row() == 1);
}

TEST_CASE("CharacterModel::sort keeps persistent indexes on their character", "[model][CharacterModel]") {
    // InitiativeView keeps the active row as a QPersistentModelIndex.
    CharacterModel model;
    auto c = makeCharacter("C", 9);
    auto b = makeCharacter("B", 6);
    auto a = makeCharacter("A", 2);
    model.addItems({c, b, a});
    QPersistentModelIndex onC(model.index(0, 0));
    QPersistentModelIndex onA(model.index(2, 0));

    model.sort(0, CombatSequenceState::RESOLUTION);

    REQUIRE(onC.isValid());
    REQUIRE(onA.isValid());
    CHECK(model.getItem(onC) == c);
    CHECK(model.getItem(onA) == a);
    CHECK(onC.row() == 2);
    CHECK(onA.row() == 0);
}

TEST_CASE("CharacterModel::sort from a row leaves earlier persistent indexes alone", "[model][CharacterModel]") {
    CharacterModel model;
    auto first = makeCharacter("First", 9);
    auto b = makeCharacter("B", 6);
    auto a = makeCharacter("A", 2);
    model.addItems({first, b, a});
    QPersistentModelIndex onFirst(model.index(0, 0));
    QPersistentModelIndex onB(model.index(1, 0));

    model.sort(1, CombatSequenceState::RESOLUTION);

    CHECK(onFirst.row() == 0);
    CHECK(model.getItem(onB) == b);
    CHECK(onB.row() == 2);
}

TEST_CASE("CharacterModel::sort maps duplicate rows of the same character", "[model][CharacterModel]") {
    // The same roster member can be added to the initiative order twice.
    CharacterModel model;
    auto twice = makeCharacter("Twice", 8);
    auto other = makeCharacter("Other", 1);
    model.addItems({twice, other, twice});
    QPersistentModelIndex row0(model.index(0, 0));
    QPersistentModelIndex row2(model.index(2, 0));

    model.sort(0, CombatSequenceState::RESOLUTION);

    CHECK(model.getItems() == QVector<std::shared_ptr<Character>>{other, twice, twice});
    CHECK(row0.row() != row2.row());
    CHECK(model.getItem(row0) == twice);
    CHECK(model.getItem(row2) == twice);
}

TEST_CASE("CharacterModel::sort keeps tied characters in their current order", "[model][CharacterModel]") {
    CharacterModel model;
    auto first = makeCharacter("First", 4);
    auto second = makeCharacter("Second", 4);
    auto third = makeCharacter("Third", 4);
    model.addItems({first, second, third});

    for (int i = 0; i < 3; ++i) model.sort(0, CombatSequenceState::RESOLUTION);

    CHECK(model.getItems() == QVector<std::shared_ptr<Character>>{first, second, third});
}

TEST_CASE("CharacterModel faction lookups follow faction changes", "[model][CharacterModel]") {
    // Roster and initiative order share Character objects, so a faction changed in
    // the roster must be seen by the initiative order.
    CharacterModel model;
    auto guard = makeCharacter("Guard", 4, Faction::Gray);
    model.appendItem(guard);

    guard->setFaction(Faction::Blue);

    CHECK(model.getItems(Faction::Blue) == QVector<std::shared_ptr<Character>>{guard});
    CHECK(model.getItems(Faction::Gray).isEmpty());
}

TEST_CASE("CharacterModel tracks the current segment and round", "[model][CharacterModel]") {
    CharacterModel model;
    auto ragnar = makeCharacter("Ragnar", 3);
    model.appendItem(ragnar);

    QList<int> rounds;
    QObject::connect(&model, &CharacterModel::roundUpdate, [&](int r) { rounds << r; });

    model.advanceInitiative(3);
    CHECK(model.data(model.index(0, 0), InitiativeRole).toInt() == 3);
    CHECK(rounds.isEmpty());

    SECTION("moving to a lower segment starts a new round and resets acted flags") {
        ragnar->setActed(true);
        model.advanceInitiative(1);
        CHECK(rounds == QList<int>{2});
        CHECK_FALSE(ragnar->hasActed());
        CHECK(model.data(model.index(0, 0), InitiativeRole).toInt() == 1);
    }
    SECTION("backtracking past the start of the round goes back a round") {
        model.advanceInitiative(1); // round 2
        model.backtrackInitiative(9);
        CHECK(rounds == QList<int>{2, 1});
    }
    SECTION("rounds never go below 1") {
        model.backtrackRound();
        model.backtrackRound();
        CHECK(rounds == QList<int>{1, 1});
    }
}
