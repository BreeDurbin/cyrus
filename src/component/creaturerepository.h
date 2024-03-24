#ifndef CREATUREREPOSITORY_H
#define CREATUREREPOSITORY_H

#include <QListView>

class CreatureRepository : public QListView
{
    Q_OBJECT

public:
    explicit CreatureRepository(QWidget *parent = nullptr);
    ~CreatureRepository();

private:
    // Add any private member variables or functions here

};

#endif // CREATUREREPOSITORY_H