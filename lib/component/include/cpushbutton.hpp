#pragma once

#include <QPushButton>
#include <QString>
#include <QWidget>

class CPushButton : public QPushButton
{
    Q_OBJECT

public:
    explicit CPushButton(const QString text = "", QWidget *parent = nullptr);
    ~CPushButton();

private:
    // Add any private member variables or functions here

};

