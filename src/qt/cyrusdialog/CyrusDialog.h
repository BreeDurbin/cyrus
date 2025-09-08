#pragma once

#include <QMainWindow>
#include <QItemSelectionModel>
#include <QAbstractItemModel>
#include "../model/CharacterModel.h"
#include "../model/InitiativeDelegate.h"
#include "../model/CombatLogDelegate.h"
#include "../view/InitiativeView.h"

QT_BEGIN_NAMESPACE
namespace Ui { class CyrusDialog; }
QT_END_NAMESPACE

class CyrusDialog 
    : public QMainWindow 
{
    Q_OBJECT

public:

    CyrusDialog(QWidget *parent = nullptr);
    ~CyrusDialog();
    void setupHeroFrame();
    void setupLabelStyleSheets();
    void setupLineEditStyleSheets();
    void setupSpinBoxStyleSheets();
    void setupButtonStyleSheets();
    void setupItemViews();
    void setupConnections();
    
private:

    //Selection Models
    QItemSelectionModel* characterSelection_; 
    QItemSelectionModel* combatLogSelection_;

    //Character models
    CharacterModel* characterModel_;
    CharacterModel* initiativeModel_;
    CharacterModel* combatLogModel_;

    //Character Delegates
    InitiativeDelegate* initiativeDelegate_;
    CombatLogDelegate* combatLogDelegate_;

    //Views
    InitiativeView* initiativeView_;

    //helpers
    void selectNext(const QAbstractItemModel* model);
    void selectPrevious(const QAbstractItemModel* model);

protected:
    Ui::CyrusDialog *ui;
};
