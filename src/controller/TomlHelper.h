#pragma once
#include <string>
#include <QVector>
#include <memory>

class Character;

namespace TomlHelper {

    void saveRepository(const std::string& filename, const QVector<std::shared_ptr<Character>>& characters);
    QVector<std::shared_ptr<Character>> loadRepository(const std::string& filename);

    
    const static std::string baseDir_ = "cyrus_data/roster/";

}