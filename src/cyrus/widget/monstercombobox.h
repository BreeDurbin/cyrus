#ifndef MONSTERCOMBOBOX_H
#define MONSTERCOMBOBOX_H

#include <QComboBox>

class MonsterComboBox : public QComboBox
{
    Q_OBJECT
public:
    MonsterComboBox(QWidget *parent = nullptr);

signals:
    void currentTextSaved(const QString &text);
};

#endif // MYCOMBOBOX_H