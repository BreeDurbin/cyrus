#include "RemovalEntry.h"

#include <QString>

QString RemovalEntry::text() const {
    return combatLog();
}

QString RemovalEntry::combatLog() const {
    return name() + QString(" was removed from combat.");
}
