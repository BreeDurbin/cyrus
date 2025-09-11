#include "TomlHelper.h"
#include <fstream>
#include "Enums.h"
#include "toml++/toml.h"
#include <filesystem>
#include <iostream>
#include "Character.h"

void TomlHelper::saveRepository(const std::string& filename,
                                const QVector<std::shared_ptr<Character>>& characters) {
    //create directory if it doesnt exist
    std::filesystem::create_directories(baseDir_);

    toml::array characterArray;

    for (const std::shared_ptr<Character> character : characters) {
        characterArray.push_back(toml::table{
            {"uuid",       character->uuid().toString(QUuid::WithoutBraces).toStdString()},
            {"name",       character->name().toStdString()},
            {"initiative", character->initiative()},
            {"class",      static_cast<int>(character->characterType())}
        });
    }

    toml::table tbl;
    tbl.insert_or_assign("characters", std::move(characterArray));

    std::ofstream file(baseDir_ + filename);
    file << tbl;
}

/*void TomlHelper::createDirectoryIfDoesntExist(){

}*/

QVector<std::shared_ptr<Character>> TomlHelper::loadRepository(const std::string& filename) {
    
    QVector<std::shared_ptr<Character>> characters;
    const std::string filepath = baseDir_ + filename;

    //filesystem check if file exists
    if (std::filesystem::exists(filepath)) {
        std::cout << "File '" << filepath << "' exists." << std::endl;
        auto tbl = toml::parse_file(filepath);
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

                characters.push_back(std::make_shared<Character>(uuid, name, initiative, static_cast<Cyrus::CharacterType>(classInt)));
            }
        }
    } else {
        std::cout << "File '" << filepath << "' does not exist." << std::endl;
    }

    return characters;
}

