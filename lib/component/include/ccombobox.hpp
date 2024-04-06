#pragma once

#include <QComboBox>

class CComboBox : public QComboBox
{
    Q_OBJECT

public:
    explicit CComboBox(QWidget *parent = nullptr);
    ~CComboBox();

private:
    // Add any private member variables or functions here

};
