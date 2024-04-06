#pragma once

#include <QListView>

class CListView : public QListView
{
    Q_OBJECT

public:
    explicit CListView(const QString text = "", QWidget *parent = nullptr);
    ~CListView();

private:
    // Add any private member variables or functions here

};
