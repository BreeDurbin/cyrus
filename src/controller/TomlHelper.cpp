#include "TomlHelper.h"
#include <fstream>
#include "Enums.h"
#include "toml++/toml.h"
#include <filesystem>
#include <iostream>
#include "Character.h"

void TomlHelper::saveRepository(const std::string& filename,
                                const QVector<std::shared_ptr<Character>>& characters) {
    // Create directory if it doesn't exist
    std::filesystem::create_directories(baseDir_);

    toml::array characterArray;

    for (const auto& character : characters) {
        characterArray.push_back(toml::table{
            {"uuid",        character->uuid().toString(QUuid::WithoutBraces).toStdString()},
            {"baseName",    character->baseName().toStdString()},
            {"class",       static_cast<int>(character->characterType())},
            {"weaponSpeed", character->weaponSpeed()},
            {"attackRate",  character->attackRate().attack()},
            {"faction",     static_cast<int>(character->faction())}
        });
    }

    toml::table tbl;
    tbl.insert_or_assign("characters", std::move(characterArray));

    std::ofstream file(baseDir_ + filename);
    file << tbl;
}

QVector<std::shared_ptr<Character>> TomlHelper::loadRepository(const std::string& filename) {
    QVector<std::shared_ptr<Character>> characters;
    const std::string filepath = baseDir_ + filename;

    if (std::filesystem::exists(filepath)) {
        std::cout << "File '" << filepath << "' exists." << std::endl;
        auto tbl = toml::parse_file(filepath);

        if (auto* arr = tbl["characters"].as_array()) {
            for (auto&& elem : *arr) {
                if (!elem.is_table())
                    continue;

                auto& characterTable = *elem.as_table();

                //need to load uuidhere
                QString baseName = QString::fromStdString(characterTable["baseName"].value_or(""));;
                int classInt = characterTable["class"].value_or(0);
                int weaponSpeed = characterTable["weaponSpeed"].value_or(0);
                int attackRateValue = characterTable["attackRate"].value_or(2); // default to 2 half attacks if missing
                int factionInt = characterTable["faction"].value_or(0); // default to red faction
                AttackRate attackRate{attackRateValue};

                // Load UUID
                QString uuidStr = QString::fromStdString(characterTable["uuid"].value_or(""));
                QUuid uuid = uuidStr.isNull() ? QUuid::createUuid() : QUuid(uuidStr);

                characters.push_back(std::make_shared<Character>(
                    uuid,
                    baseName,
                    0,
                    attackRate,
                    weaponSpeed,
                    static_cast<Cyrus::CharacterType>(classInt),
                    Cyrus::ActionType::Unset,
                    static_cast<Cyrus::Faction>(factionInt)
                ));
            }
        }
    } else {
        std::cout << "File '" << filepath << "' does not exist." << std::endl;
    }

    return characters;
}
