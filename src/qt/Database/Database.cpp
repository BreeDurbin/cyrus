#include "Database.h"
#include <QCoreApplication>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

Database::Database(QWidget *parent) :
    QWidget(parent)
{
    DatabaseConnect();
    DatabaseInit();
    DatabasePopulate();
}

Database::~Database()
{
    db.close();
}

QSqlDatabase Database::getDatabase()
{
    return db;
}

void Database::DatabaseConnect()
{
    const QString DRIVER("QSQLITE");

	if(QSqlDatabase::isDriverAvailable(DRIVER))
	{
		db = QSqlDatabase::addDatabase(DRIVER);
        QString fPath = QCoreApplication::applicationDirPath();
        auto fileName = fPath + "/cyrus.db";
		db.setDatabaseName(fileName);
        db.setUserName("cyrus");
        db.setPassword("password");
        bool ok = db.open();

		if(!db.open())
			qWarning() << "Database::DatabaseConnect - ERROR: " << db.lastError().text();
	}
	else
		qWarning() << "Database::DatabaseConnect - ERROR: no driver " << DRIVER << " available";


}

void Database::DatabaseInit()
{
    QSqlQuery query;
    //creatures are a registry of all types that can be added easitly to encounters
    query.exec("CREATE TABLE IF NOT EXISTS creature (id INTEGER PRIMARY KEY, name TEXT, type TEXT, description TEXT, type TEXT, initiative INTEGER))");
    //effects are things that can happen to creatures or the world. They have a type, duration, and target
    //target can be a creature or a location/area
    query.exec("CREATE TABLE IF NOT EXISTS effect (id INTEGER PRIMARY KEY, type TEXT, duration INT, target INTEGER)");
    query.exec("CREATE TABLE IF NOT EXISTS creature_effect (id INTEGER PRIMARY KEY, creature_id INTEGER, effect_id INTEGER)");
    //each encounter is a list of creatures
    query.exec("CREATE TABLE IF NOT EXISTS encounter (id INTEGER PRIMARY KEY, name TEXT, description TEXT)");
    query.exec("CREATE TABLE IF NOT EXISTS encounter_creature (id INTEGER PRIMARY KEY, encounter_id INTEGER, creature_id INTEGER)");
    // the world to be displayed, includes things like time of day, weather, and other environmental factors
    query.exec("CREATE TABLE IF NOT EXISTS world (id INTEGER PRIMARY KEY, name TEXT, plane TEXT, description TEXT)");
    //campaigns are associated with encounters, worlds and events and have a speciofied rules system
    query.exec("CREATE TABLE IF NOT EXISTS campaign (id INTEGER PRIMARY KEY, name TEXT, cosmology, description TEXT)");
    query.exec("CREATE TABLE IF NOT EXISTS campaign_encounter (id INTEGER PRIMARY KEY, campaign_id INTEGER, encounter_id INTEGER)");
    query.exec("CREATE TABLE IF NOT EXISTS campaign_world (id INTEGER PRIMARY KEY, campaign_id INTEGER, world_id INTEGER)");
    query.exec("CREATE TABLE IF NOT EXISTS campaign_event (id INTEGER PRIMARY KEY, campaign_id INTEGER, event_id INTEGER)");
    //events are things that happen in the world, they can be triggered by time, location, or other events
    query.exec("CREATE TABLE IF NOT EXISTS event (id INTEGER PRIMARY KEY, name TEXT, start INTEGER, end INTEGER, world_id INTEGER, description TEXT)");
    query.exec("CREATE TABLE IF NOT EXISTS world_event (id INTEGER PRIMARY KEY, event_id INTEGER, world_id INTEGER)");
}

void Database::DatabasePopulate()
{
    QSqlQuery query;
    //creates default campaign, homebrew 
    query.exec("INSERT INTO campaign (id, name, cosmology, description) VALUES (0, 'Homebrew', 'Homebrew Cosmology' 'Homebrew campaign')");
    //creates a default encounter
    query.exec("INSERT INTO encounter (id, name, description) VALUES (0, 'Default', 'Default encounter')");
    //seed worlds
    query.exec("INSERT INTO world (id, name, plane, description) VALUES (0, 'Homebrew', 'Homebrew Plane', 'Homebrew world')");

    //associates the default campaign with the default encounter and world
    query.exec("INSERT INTO campaign_encounter (id, campaign_id, encounter_id) VALUES (0, 0)");
    query.exec("INSERT INTO campaign_world (id, campaign_id, world_id) VALUES (0, 0)");

    //seed creature data
    query.exec("INSERT INTO creature (id, name, type, description, initiative) VALUES (0, 'Goblin', 'Humanoid', 'A small, green creature', 1)");
    query.exec("INSERT INTO creature (id, name, type, description, initiative) VALUES (1, 'Orc', 'Humanoid', 'A large, green creature', 2)");
    query.exec("INSERT INTO creature (id, name, type, description, initiative) VALUES (2, 'Dragon', 'Dragon', 'A large, fire breathing creature', 3)");
    // tiie creatures to encounter 0
    query.exec("INSERT INTO encounter_creature (id, encounter_id, creature_id) VALUES (0, 0, 0)");
    query.exec("INSERT INTO encounter_creature (id, encounter_id, creature_id) VALUES (1, 0, 1)");
    query.exec("INSERT INTO encounter_creature (id, encounter_id, creature_id) VALUES (2, 0, 2)");
    //seed effect data
    query.exec("INSERT INTO effect (id, type, duration, target) VALUES (0, 'AOE', 1, 0)");
    //seed event date
    query.exec("INSERT INTO event (id, name, description) VALUES (0, 'Default Event', 'Default event')");
    //associate event with world
    query.exec("INSERT INTO world_event (id, event_id, world_id) VALUES (0, 0, 0)");

}
