#include "TestHelpers.h"

#include <filesystem>
#include <fstream>

#include "TomlHelper.h"
#include "Character.h"

using namespace Cyrus;
using TestHelpers::ScopedTempWorkingDir;

namespace {

void writeRosterFile(const std::string& filename, const std::string& contents) {
    std::filesystem::create_directories(TomlHelper::baseDir_);
    std::ofstream(TomlHelper::baseDir_ + filename) << contents;
}

// Copies a roster file into the current (temporary) working directory's roster dir.
void installRoster(const std::filesystem::path& sourceDir, const std::string& filename) {
    std::filesystem::create_directories(TomlHelper::baseDir_);
    std::filesystem::copy_file(sourceDir / filename,
                               std::filesystem::path(TomlHelper::baseDir_) / filename);
}

std::string readFile(const std::filesystem::path& path) {
    std::ifstream file(path);
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

}

TEST_CASE("TomlHelper saves a roster under cyrus_data/roster", "[controller][TomlHelper]") {
    ScopedTempWorkingDir tmp;
    QVector<std::shared_ptr<Character>> roster{TestHelpers::makeCharacter("Ragnar", 3)};

    TomlHelper::saveRepository("party.toml", roster);

    CHECK(std::filesystem::exists(tmp.path() / "cyrus_data" / "roster" / "party.toml"));
}

TEST_CASE("TomlHelper round-trips roster members", "[controller][TomlHelper]") {
    ScopedTempWorkingDir tmp;
    auto ragnar = std::make_shared<Character>("Ragnar", 3, AttackRate{2}, 4,
                                              CharacterType::Fighter, ActionType::Attack, Faction::Blue);
    auto orc = std::make_shared<Character>("Orc", 7, AttackRate{2}, 8,
                                           CharacterType::Monster, ActionType::Unset, Faction::Red);

    TomlHelper::saveRepository("party.toml", {ragnar, orc});
    auto loaded = TomlHelper::loadRepository("party.toml");

    REQUIRE(loaded.size() == 2);
    CHECK(loaded[0]->uuid() == ragnar->uuid());
    CHECK(loaded[0]->baseName() == QString("Ragnar"));
    CHECK(loaded[0]->characterType() == CharacterType::Fighter);
    CHECK(loaded[0]->weaponSpeed() == 4);
    CHECK(loaded[0]->faction() == Faction::Blue);

    CHECK(loaded[1]->uuid() == orc->uuid());
    CHECK(loaded[1]->baseName() == QString("Orc"));
    CHECK(loaded[1]->characterType() == CharacterType::Monster);
    CHECK(loaded[1]->weaponSpeed() == 8);
    CHECK(loaded[1]->faction() == Faction::Red);
}

TEST_CASE("TomlHelper does not persist per-combat state", "[controller][TomlHelper]") {
    ScopedTempWorkingDir tmp;
    auto ragnar = std::make_shared<Character>("Ragnar", 3, AttackRate{2}, 4,
                                              CharacterType::Fighter, ActionType::Attack, Faction::Blue);

    TomlHelper::saveRepository("party.toml", {ragnar});
    auto loaded = TomlHelper::loadRepository("party.toml");

    REQUIRE(loaded.size() == 1);
    CHECK(loaded[0]->initiative() == 0);
    CHECK(loaded[0]->actionType() == ActionType::Unset);
}

TEST_CASE("TomlHelper round-trips attack rates", "[controller][TomlHelper]") {
    // Regression: saveRepository used to write attackRate().attack() (attacks this
    // round) and loadRepository read it back as half attacks, so 1 attack/round
    // saved as 1 and loaded as 1/2.
    ScopedTempWorkingDir tmp;
    auto halfAttacks = GENERATE(2, 3, 4, 5);
    auto fighter = TestHelpers::makeCharacter("Ragnar", 3, Faction::Blue, halfAttacks);

    TomlHelper::saveRepository("party.toml", {fighter});
    auto loaded = TomlHelper::loadRepository("party.toml");

    REQUIRE(loaded.size() == 1);
    INFO("saved " << fighter->attackRate().toString().toStdString());
    CHECK(loaded[0]->attackRate().halfAttacks_ == halfAttacks);
}

TEST_CASE("TomlHelper saves attack rates as half attacks", "[controller][TomlHelper]") {
    ScopedTempWorkingDir tmp;
    TomlHelper::saveRepository("party.toml", {TestHelpers::makeCharacter("Ragnar", 3, Faction::Blue, 5)});

    std::ifstream file(TomlHelper::baseDir_ + "party.toml");
    const std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    CHECK(contents.find("halfAttacks = 5") != std::string::npos);
    CHECK(contents.find("attackRate") == std::string::npos);
}

TEST_CASE("TomlHelper ignores the old attackRate key", "[controller][TomlHelper]") {
    // The pre-halfAttacks format is no longer read; such entries get the default.
    ScopedTempWorkingDir tmp;
    writeRosterFile("old.toml",
        "[[characters]]\n"
        "baseName = 'Ragnar'\n"
        "attackRate = 3\n");

    auto loaded = TomlHelper::loadRepository("old.toml");

    REQUIRE(loaded.size() == 1);
    CHECK(loaded[0]->attackRate().halfAttacks_ == 2);
}

TEST_CASE("TomlHelper clamps attack rates to 1..10 attacks", "[controller][TomlHelper]") {
    ScopedTempWorkingDir tmp;
    auto [stored, halfAttacks] = GENERATE(table<int, int>({
        {0, 2},
        {1, 2},
        {-4, 2},
        {21, 20},
        {99, 20},
    }));
    writeRosterFile("clamp.toml",
        "[[characters]]\n"
        "baseName = 'Ragnar'\n"
        "halfAttacks = " + std::to_string(stored) + "\n");

    auto loaded = TomlHelper::loadRepository("clamp.toml");

    REQUIRE(loaded.size() == 1);
    INFO("stored halfAttacks = " << stored);
    CHECK(loaded[0]->attackRate().halfAttacks_ == halfAttacks);
}

TEST_CASE("TomlHelper generates a uuid when the stored one is unreadable", "[controller][TomlHelper]") {
    ScopedTempWorkingDir tmp;
    writeRosterFile("bad_uuid.toml",
        "[[characters]]\n"
        "baseName = 'Nameless'\n"
        "uuid = 'not-a-uuid'\n"
        "\n"
        "[[characters]]\n"
        "baseName = 'Nameless Too'\n"
        "uuid = 'not-a-uuid'\n");

    auto loaded = TomlHelper::loadRepository("bad_uuid.toml");

    REQUIRE(loaded.size() == 2);
    CHECK_FALSE(loaded[0]->uuid().isNull());
    CHECK_FALSE(loaded[1]->uuid().isNull());
    CHECK(loaded[0]->uuid() != loaded[1]->uuid());
}

TEST_CASE("TomlHelper loading a missing roster returns nothing", "[controller][TomlHelper]") {
    ScopedTempWorkingDir tmp;
    CHECK(TomlHelper::loadRepository("does_not_exist.toml").isEmpty());
}

TEST_CASE("TomlHelper fills in defaults for missing fields", "[controller][TomlHelper]") {
    ScopedTempWorkingDir tmp;
    writeRosterFile("sparse.toml",
        "[[characters]]\n"
        "baseName = 'Nameless'\n"
        "uuid = '7ca6268d-1f0a-4bba-bb62-227e8375a1bc'\n");

    auto loaded = TomlHelper::loadRepository("sparse.toml");

    REQUIRE(loaded.size() == 1);
    CHECK(loaded[0]->baseName() == QString("Nameless"));
    CHECK(loaded[0]->characterType() == CharacterType::Barbarian); // class 0
    CHECK(loaded[0]->weaponSpeed() == 0);
    CHECK(loaded[0]->faction() == Faction::Red);
    CHECK(loaded[0]->attackRate().halfAttacks_ == 2);
}

TEST_CASE("TomlHelper generates a uuid when one is missing", "[controller][TomlHelper]") {
    // Regression: the fallback checked uuidStr.isNull(), but fromStdString("") is
    // empty, not null, so a missing uuid loaded as a null QUuid.
    ScopedTempWorkingDir tmp;
    writeRosterFile("no_uuid.toml",
        "[[characters]]\n"
        "baseName = 'Nameless'\n");

    auto loaded = TomlHelper::loadRepository("no_uuid.toml");

    REQUIRE(loaded.size() == 1);
    CHECK_FALSE(loaded[0]->uuid().isNull());
}

TEST_CASE("TomlHelper skips entries that are not tables", "[controller][TomlHelper]") {
    ScopedTempWorkingDir tmp;
    writeRosterFile("mixed.toml",
        "characters = [ 'not a table', { baseName = 'Guts', faction = 1 } ]\n");

    auto loaded = TomlHelper::loadRepository("mixed.toml");

    REQUIRE(loaded.size() == 1);
    CHECK(loaded[0]->baseName() == QString("Guts"));
    CHECK(loaded[0]->faction() == Faction::Blue);
}

TEST_CASE("TomlHelper loads a saved roster from gold data", "[controller][TomlHelper]") {
    ScopedTempWorkingDir tmp;
    installRoster(CYRUS_GOLD_DATA_DIR, "Band of the Hawk.toml");

    auto loaded = TomlHelper::loadRepository("Band of the Hawk.toml");

    REQUIRE(loaded.size() == 7);
    QStringList names;
    for (const auto& c : loaded) {
        names << c->baseName();
        CHECK(c->faction() == Faction::Blue);
        CHECK(c->attackRate().halfAttacks_ == 2);
    }
    CHECK(names == QStringList{"Guts", "Casca", "Griffith", "Judeau", "Pippin", "Corkus", "Rickert"});
    CHECK(loaded[0]->uuid() == QUuid("7ca6268d-1f0a-4bba-bb62-227e8375a1bc"));
}

TEST_CASE("TomlHelper loads an encounter roster from gold data", "[controller][TomlHelper]") {
    ScopedTempWorkingDir tmp;
    installRoster(CYRUS_GOLD_DATA_DIR, "Encounter 1.toml");

    auto loaded = TomlHelper::loadRepository("Encounter 1.toml");

    REQUIRE(loaded.size() == 7);
    for (int i = 0; i < 6; ++i) CHECK(loaded[i]->baseName() == QString("Goblin"));
    CHECK(loaded[6]->baseName() == QString("Goblin Shaman"));
    for (const auto& c : loaded) {
        INFO(c->baseName().toStdString());
        CHECK(c->faction() == Faction::Red);
        CHECK(c->attackRate().halfAttacks_ == 2);
    }
    // Same base name, so the goblins are numbered on load.
    CHECK(loaded[0]->name() == QString("Goblin 1"));
    CHECK(loaded[5]->name() == QString("Goblin 6"));
    CHECK(loaded[6]->name() == QString("Goblin Shaman"));
}

TEST_CASE("Gold data rosters are in the current format", "[controller][TomlHelper]") {
    const std::filesystem::path goldDir(CYRUS_GOLD_DATA_DIR);
    int files = 0;
    for (const auto& entry : std::filesystem::directory_iterator(goldDir)) {
        if (entry.path().extension() != ".toml") continue;
        ++files;
        const std::string contents = readFile(entry.path());
        INFO(entry.path().filename().string());
        CHECK(contents.find("halfAttacks") != std::string::npos);
        CHECK(contents.find("attackRate") == std::string::npos);
    }
    CHECK(files == 4);
}

TEST_CASE("Gold data rosters re-save byte-for-byte", "[controller][TomlHelper]") {
    // Loading and saving a current-format roster must not change it.
    const std::string filename = GENERATE(
        std::string("Band of the Hawk.toml"), std::string("default_name.toml"),
        std::string("Encounter 1.toml"), std::string("Hero's Party.toml"));
    ScopedTempWorkingDir tmp;
    installRoster(CYRUS_GOLD_DATA_DIR, filename);

    TomlHelper::saveRepository(filename, TomlHelper::loadRepository(filename));

    INFO(filename);
    CHECK(readFile(std::filesystem::path(TomlHelper::baseDir_) / filename) ==
          readFile(std::filesystem::path(CYRUS_GOLD_DATA_DIR) / filename));
}
