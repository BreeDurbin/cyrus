#pragma once
#include <QMetaType>

class QString;


class Creature 
{

public:
    enum Role { PC, NPC, Monster };

    Creature() = default;
    Creature(QString name, Role role, int initiative);

    QString name() const;
    Role role() const;
    int initiative() const;

    QString toString() const;
    static int qtMetaType;

private:
    QString m_name = "";
    Role m_role = Monster;
    int m_initiative = 0;
};

Q_DECLARE_METATYPE(Creature)