#pragma once

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
