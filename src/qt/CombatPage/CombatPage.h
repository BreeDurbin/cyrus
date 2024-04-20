#pragma once

#include <QWidget>
#include <QListView>

QT_BEGIN_NAMESPACE
namespace Ui { class CombatPage; }
QT_END_NAMESPACE


class CombatPage : public QWidget
{
    Q_OBJECT

public:
    explicit CombatPage(QWidget *parent = nullptr);
    ~CombatPage();
    void initializeInitiative();
    void initializeEncounter();

private:
    Ui::CombatPage *ui;

};
