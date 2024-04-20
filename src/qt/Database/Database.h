#pragma once

#include <QWidget>
#include <QSqlDatabase>

class Database : public QWidget
{
    Q_OBJECT

public:
    explicit Database(QWidget *parent = nullptr);
    ~Database();

    void DatabaseConnect();
	void DatabaseInit();
	void DatabasePopulate();
    QSqlDatabase getDatabase();

private:
    QSqlDatabase db;
};
