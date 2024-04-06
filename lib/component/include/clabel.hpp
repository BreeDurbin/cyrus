#pragma once

#include <QLabel>

class CLabel : public QLabel
{
    Q_OBJECT

public:
    explicit CLabel(const QString text = "", QWidget *parent = nullptr);
    ~CLabel();

private:
    // Add any private member variables or functions here

};
