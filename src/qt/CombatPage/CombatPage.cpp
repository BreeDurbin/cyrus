#include "CombatPage.h"
#include "ui_CombatPage.h"

#include <clabel.h>
#include <cpushbutton.h>
#include <ctableview.h>
#include "CreatureModel.h"
#include <ctabwidget.h>
#include <cstyleditemdelegate.h>

CombatPage::CombatPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CombatPage)
{
    ui->setupUi(this);
    ui->encounterListView->viewport()->setAutoFillBackground(false);
    ui->initiativeListView->viewport()->setAutoFillBackground(false);
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
    auto itemDelegate = new CStyledItemDelegate();
    ui->initiativeListView->setItemDelegate(itemDelegate);
    auto initiativeList = new CreatureModel();
    ui->initiativeListView->setModel(initiativeList);

    //connections
    
    //sorts the initiativeList
    connect(ui->initiativeSortButton, &QPushButton::clicked, initiativeList, &CreatureModel::sort);
    //connects the next button to the nextRow slot of the initiativeList
    //if the next row wraps aroudn to the beginning then that is a new round also
    connect(ui->initiativeNextButton, &QPushButton::clicked, initiativeList, &CreatureModel::nextRow);
    //connects the newRound signal to the setNum slot of the initiativeRoundCounter
    connect(initiativeList, &CreatureModel::newRound, ui->initiativeRoundCounter, qOverload<int>(&QLabel::setNum));
    //start a new combat
    connect(ui->initiativeNewCombatButton, &QPushButton::clicked, initiativeList, &CreatureModel::newCombat);

}

void CombatPage::initializeEncounter(){

    // Player Character Repository
    auto itemDelegate = new CStyledItemDelegate();
    ui->encounterListView->setItemDelegate(itemDelegate);
    auto creatureList = new CreatureModel();
    ui->encounterListView->setModel(creatureList);

    //connections
    connect(ui->encounterAddButton, &QPushButton::clicked, [creatureList](){
        CreatureItem temp;
        temp.name = "New PC";
        temp.type = "PC";
        temp.level = 1;
        temp.initiative = 0;
        creatureList->addRows(temp);
    });


    //TODO: not working yet
    //move all cresatures in the encounterListView to the initiativeListView
    connect(ui->encounterMoveAllButton, &QPushButton::clicked, [this](){
        auto creatureList = qobject_cast<CreatureModel*>(ui->encounterListView->model())->getCreatures();
        qobject_cast<CreatureModel*>(ui->encounterListView->model())->addCreatures(creatureList);
    });

}