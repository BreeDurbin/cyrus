#include "CombatPage.h"
#include "ui_CombatPage.h"

#include <clabel.h>
#include <cpushbutton.h>
#include <ctableview.h>
#include "Creature.h"
#include "CreatureModel.h"
#include <ctabwidget.h>
#include "CreatureDelegate.h"

CombatPage::CombatPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CombatPage)
{
    ui->setupUi(this);
    ui->encounterTableView->viewport()->setAutoFillBackground(false);
    ui->initiativeTableView->viewport()->setAutoFillBackground(false);
    initializeEncounter();
    initializeInitiative();
}

CombatPage::~CombatPage()
{
    delete ui;
}

void CombatPage::initializeInitiative(){
    //persian gold 235, 180, 15
    // deep gold 194, 155, 12
    auto itemDelegate = new CreatureDelegate();
    ui->initiativeTableView->setItemDelegate(itemDelegate);
    auto initiativeTableModel = new CreatureModel();
    ui->initiativeTableView->setModel(initiativeTableModel);

    //connections
    
    //sorts the initiativeList
    connect(ui->initiativeSortButton, &QPushButton::clicked, initiativeTableModel, &CreatureModel::sort);
    //connects the next button to the nextRow slot of the initiativeList
    //if the next row wraps aroudn to the beginning then that is a new round also
    connect(ui->initiativeNextButton, &QPushButton::clicked, initiativeTableModel, &CreatureModel::nextRow);
    //connects the newRound signal to the setNum slot of the initiativeRoundCounter
    connect(initiativeTableModel, &CreatureModel::newRound, ui->initiativeRoundCounter, qOverload<int>(&QLabel::setNum));
    //start a new combat
    connect(ui->initiativeNewCombatButton, &QPushButton::clicked, initiativeTableModel, &CreatureModel::newCombat);

}

void CombatPage::initializeEncounter(){

    // Player Character Repository
    auto itemDelegate = new CreatureDelegate();
    ui->encounterTableView->setItemDelegate(itemDelegate);
    auto creatureTableModel = new CreatureModel();
    ui->encounterTableView->setModel(creatureTableModel);

    //connections
    connect(ui->encounterAddButton, &QPushButton::clicked, [creatureTableModel](){
        auto creature = new Creature("New Pc", Creature::Role::PC, 0);
        creatureTableModel->addRows(*creature);
    });


    //TODO: not working yet
    //move all cresatures in the encounterListView to the initiativeListView
    connect(ui->encounterMoveAllButton, &QPushButton::clicked, [this](){
        auto creatureList = qobject_cast<CreatureModel*>(ui->encounterTableView->model())->getCreatures();
        qobject_cast<CreatureModel*>(ui->encounterTableView->model())->addCreatures(creatureList);
    });

}