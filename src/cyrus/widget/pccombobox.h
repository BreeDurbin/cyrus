#ifndef PCCOMBOBOX_H
#define PCCOMBOBOX_H

#include <QComboBox>

class PcComboBox : public QComboBox
{
    Q_OBJECT
public:
    PcComboBox(QWidget *parent = nullptr);

signals:
    void currentTextSaved(const QString &text);
};

#endif // MYCOMBOBOX_H