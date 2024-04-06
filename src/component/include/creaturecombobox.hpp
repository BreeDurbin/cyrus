#pragma once

#include <CComboBox>

class CreatureComboBox : public CComboBox
{
    Q_OBJECT
public:
    CreatureComboBox(QWidget *parent = nullptr);

signals:
    void currentTextSaved(const QString &text);
};
