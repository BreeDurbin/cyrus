#pragma once

#include<QTabWidget>

class CTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit CTabWidget(const QString text = "", QWidget *parent = nullptr);
    ~CTabWidget();

private:
    // Add any private member variables or functions here

};
