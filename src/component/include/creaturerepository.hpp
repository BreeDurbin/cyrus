#pragma once

#include <QTableView>

class CreatureRepository : public QTableView
{
    Q_OBJECT

public:
    explicit CreatureRepository(QWidget *parent = nullptr);
    ~CreatureRepository();

private:
    // Add any private member variables or functions here

};
