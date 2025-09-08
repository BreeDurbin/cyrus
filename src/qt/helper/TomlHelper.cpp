#include "TomlHelper.h"
#include <toml++/toml.h>
#include <fstream>
#include "Enums.h"



void TomlHelper::saveRepository(const std::string& filename,
                                const QVector<Character>& characters) {
    toml::array characterArray;

    for (const Character& character : characters) {
        characterArray.push_back(toml::table{
            {"uuid",       character.uuid().toString(QUuid::WithoutBraces).toStdString()},
            {"name",       character.name().toStdString()},
            {"initiative", character.initiative()},
            {"class",      static_cast<int>(character.characterType())}
        });
    }

    toml::table tbl;
    tbl.insert_or_assign("characters", std::move(characterArray));

    std::ofstream file(filename);
    file << tbl;
}

QVector<Character> TomlHelper::loadRepository(const std::string& filename) {
    auto tbl = toml::parse_file(filename);
    QVector<Character> characters;

    if (auto* arr = tbl["characters"].as_array()) {
        for (auto&& elem : *arr) {
            if (!elem.is_table())
                continue;

            auto& characterTable = *elem.as_table();

            QString name = QString::fromStdString(characterTable["name"].value_or(""));
            int initiative = characterTable["initiative"].value_or(0);
            int classInt = characterTable["class"].value_or(0);

            // Try to load UUID
            QString uuidStr = QString::fromStdString(characterTable["uuid"].value_or(""));
            QUuid uuid = uuidStr.isNull() ? QUuid::createUuid() : QUuid(uuidStr);

            characters.push_back(Character{uuid, name, initiative,
                                           static_cast<Cyrus::CharacterType>(classInt)});
        }
    }

    return characters;
}

