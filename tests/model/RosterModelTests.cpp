#include "TestHelpers.h"

#include "RosterModel.h"

using namespace Cyrus;
using TestHelpers::makeCharacter;

TEST_CASE("RosterModel always has a trailing sentinel row", "[model][RosterModel]") {
    RosterModel model;
    REQUIRE(model.rowCount() == 1);

    const QModelIndex sentinelIdx = model.index(0, 0);
    auto sentinel = model.data(sentinelIdx, CharacterRole).value<std::shared_ptr<Character>>();
    REQUIRE(sentinel);
    CHECK(sentinel->isSentinel());
    CHECK(model.flags(sentinelIdx).testFlag(Qt::ItemIsEditable));
    CHECK(model.getItem(sentinelIdx) == nullptr);

    model.appendItem(makeCharacter("Ragnar", 0));
    CHECK(model.rowCount() == 2);
    CHECK(model.data(model.index(1, 0), CharacterRole)
              .value<std::shared_ptr<Character>>()->isSentinel());
}

TEST_CASE("RosterModel exposes members through its roles", "[model][RosterModel]") {
    RosterModel model;
    auto ragnar = makeCharacter("Ragnar", 0);
    model.appendItem(ragnar);
    const QModelIndex idx = model.index(0, 0);

    CHECK(model.data(idx, Qt::DisplayRole).toString() == QString("Ragnar"));
    CHECK(model.data(idx, CharacterRole).value<std::shared_ptr<Character>>() == ragnar);
    CHECK(model.flags(idx).testFlag(Qt::ItemIsSelectable));
    CHECK_FALSE(model.flags(idx).testFlag(Qt::ItemIsEditable));
}

TEST_CASE("RosterModel adds, replaces and removes members", "[model][RosterModel]") {
    RosterModel model;
    auto a = makeCharacter("Ragnar", 0);
    auto b = makeCharacter("Orc", 0);
    auto c = makeCharacter("Mouse", 0);
    model.appendItem(a);
    model.addItems({b, c});
    REQUIRE(model.getItems() == QVector<std::shared_ptr<Character>>{a, b, c});

    auto d = makeCharacter("Guard", 0);
    model.setItem(model.index(1, 0), d);
    CHECK(model.getItem(model.index(1, 0)) == d);

    model.removeItem(model.index(0, 0));
    CHECK(model.getItems() == QVector<std::shared_ptr<Character>>{d, c});

    CHECK(model.removeRows(0, 2));
    CHECK(model.getItems().isEmpty());
    CHECK(model.rowCount() == 1);
}

TEST_CASE("RosterModel ignores invalid edits", "[model][RosterModel]") {
    RosterModel model;
    auto a = makeCharacter("Ragnar", 0);
    model.appendItem(a);

    CHECK_FALSE(model.removeRows(0, 2));     // would remove the sentinel
    CHECK_FALSE(model.removeRows(-1, 1));
    model.removeItem(model.index(1, 0));     // sentinel row
    model.setItem(model.index(1, 0), makeCharacter("Orc", 0));
    model.addItems({});

    CHECK(model.getItems() == QVector<std::shared_ptr<Character>>{a});
}

TEST_CASE("RosterModel filters members by faction", "[model][RosterModel]") {
    RosterModel model;
    auto pc = makeCharacter("Ragnar", 0, Faction::Blue);
    auto orc = makeCharacter("Orc", 0, Faction::Red);
    model.addItems({pc, orc});

    CHECK(model.getItems(Faction::Blue) == QVector<std::shared_ptr<Character>>{pc});
    CHECK(model.getItems(Faction::Red) == QVector<std::shared_ptr<Character>>{orc});
    CHECK(model.getItems(Faction::Gray).isEmpty());
}

TEST_CASE("RosterModel::clear removes every member", "[model][RosterModel]") {
    RosterModel model;
    model.addItems({makeCharacter("Ragnar", 0), makeCharacter("Orc", 0)});
    model.clear();
    CHECK(model.getItems().isEmpty());
    CHECK(model.rowCount() == 1);
}
