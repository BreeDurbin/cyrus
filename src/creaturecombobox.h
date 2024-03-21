#ifndef CREATURECOMBOBOX_H
#define CREATURECOMBOBOX_H

#include <QLineEdit>
#include <QComboBox>

class CreatureComboBox : public QComboBox
{
    Q_OBJECT
public:
    CreatureComboBox(QWidget *parent = nullptr);

signals:
    void currentTextSaved(const QString &text);
};

#endif // MYCOMBOBOX_H