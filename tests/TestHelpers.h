#pragma once

#include <catch_amalgamated.hpp>

#include <QString>
#include <QUuid>
#include <filesystem>
#include <memory>
#include <ostream>

#include "Character.h"
#include "Enums.h"

// Let Catch2 print QStrings in failure messages.
inline std::ostream& operator<<(std::ostream& os, const QString& s) {
    return os << '"' << s.toStdString() << '"';
}

namespace TestHelpers {

inline std::shared_ptr<Character> makeCharacter(
    const QString& name,
    int initiative,
    Cyrus::Faction faction = Cyrus::Faction::Red,
    int halfAttacks = 2,
    int weaponSpeed = 0)
{
    return std::make_shared<Character>(
        name, initiative, AttackRate{halfAttacks}, weaponSpeed,
        Cyrus::CharacterType::Fighter, Cyrus::ActionType::Unset, faction);
}

// Runs a test inside a fresh temporary working directory, since TomlHelper
// reads and writes cyrus_data/roster/ relative to the current directory.
class ScopedTempWorkingDir {
public:
    ScopedTempWorkingDir()
        : previous_(std::filesystem::current_path()),
          dir_(std::filesystem::temp_directory_path() /
               ("cyrus_tests_" + QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString()))
    {
        std::filesystem::create_directories(dir_);
        std::filesystem::current_path(dir_);
    }

    ~ScopedTempWorkingDir() {
        std::error_code ec;
        std::filesystem::current_path(previous_, ec);
        std::filesystem::remove_all(dir_, ec);
    }

    ScopedTempWorkingDir(const ScopedTempWorkingDir&) = delete;
    ScopedTempWorkingDir& operator=(const ScopedTempWorkingDir&) = delete;

    const std::filesystem::path& path() const { return dir_; }

private:
    std::filesystem::path previous_;
    std::filesystem::path dir_;
};

}
