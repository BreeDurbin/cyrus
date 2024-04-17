#pragma once

#include <QTableView>

class CTableView : public QTableView
{
    Q_OBJECT

public:
    explicit CTableView(const QString text = "", QWidget *parent = nullptr);
    ~CTableView();

private:
    // Add any private member variables or functions here

};
