#include <string>
#include <QVector>
#include "../model/Character.h"

namespace TomlHelper {

    void saveRepository(const std::string& filename, const QVector<Character>& characters);
    QVector<Character> loadRepository(const std::string& filename);

}