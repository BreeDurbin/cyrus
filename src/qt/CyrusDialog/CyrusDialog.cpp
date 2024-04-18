#include "CyrusDialog.h"
#include "ui_CyrusDialog.h"

#include "../CombatPage/CombatPage.h"
#include "../CampaignPage/CampaignPage.h"
#include "../CreaturePage/CreaturePage.h"
#include "../SettingsPage/SettingsPage.h"
#include "../WorldPage/WorldPage.h"

#include <iostream>

#include <QStringList>
#include <QGroupBox>
#include <QTextEdit>
#include <QStringListModel>

#include <clabel.h>
#include <cpushbutton.h>
#include <ctableview.h>
#include "CreatureModel.h"
#include <ctabwidget.h>
#include <cstyleditemdelegate.h>


CyrusDialog::CyrusDialog(QWidget *parent) : QMainWindow(parent), ui(new Ui::CyrusDialog)
{
    ui->setupUi(this);

    //title and size
    this->setWindowTitle("Cyrus: Combat Tracker");

    setStyleSheet("#MainWindow { border-image: url(:/images/cyrus_background.png) 0 0 0 0 stretch stretch;}");

    //tab setup
    addCombatPage();
    addCreaturePage();
    addWorldPage();
    addCampaignPage();
    addSettingsPage();

    /*
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    //setBackgroundRole(QPalette::Background);

    // core layout
    QHBoxLayout *outer = new QHBoxLayout();

    //tab setup
    CTabWidget *tabs = new CTabWidget();
    tabs->setDocumentMode(true);
    auto combatTab = new QWidget();
    combatTab->setAutoFillBackground(false);
    combatTab->setLayout(outer);
    tabs->addTab(combatTab, "Combat");
    tabs->addTab(new QWidget(), "Bestiary");
    tabs->addTab(new QWidget(), "World");
    tabs->addTab(new QWidget(), "Settings");

    setCentralWidget(tabs);

    // initialization calls
    initializeInitiative(outer);
    initializeCreatureRepository(outer);

    // final show call
    this->centralWidget()->show(); */
}

CyrusDialog::~CyrusDialog()
{
    delete ui;
}


void CyrusDialog::addCombatPage()
{
    ui->tabWidget->addTab(new CombatPage(), "Combat");
}


void CyrusDialog::addCreaturePage()
{
    ui->tabWidget->addTab(new CreaturePage(), "Creature");
}


void CyrusDialog::addWorldPage()
{
    ui->tabWidget->addTab(new WorldPage(), "World");
}


void CyrusDialog::addCampaignPage()
{
    ui->tabWidget->addTab(new CombatPage(), "Campaign");
}


void CyrusDialog::addSettingsPage()
{
    ui->tabWidget->addTab(new SettingsPage(), "Settings");
}

void CyrusDialog::initializeInitiative(QHBoxLayout* outer){
    //persian gold 235, 180, 15
    // deep gold 194, 155, 12
    QVBoxLayout *inner = new QVBoxLayout;
    outer->addLayout(inner);

    auto banner = new CLabel("Initiative Tracker");
    inner->addWidget(banner);

    auto *initiativeRepository = new CTableView();
    auto itemDelegate = new CStyledItemDelegate();
    initiativeRepository->setItemDelegate(itemDelegate);
    auto initiativeList = new CreatureModel();
    initiativeRepository->setModel(initiativeList);
    inner->addWidget(initiativeRepository);
    initiativeRepository->show();

    QHBoxLayout *innerBottom = new QHBoxLayout;
    inner->addLayout(innerBottom);

    CPushButton *sortButton = new CPushButton("Sort");
    QObject::connect(sortButton, &QPushButton::clicked, initiativeList, &CreatureModel::sort);
    innerBottom->addWidget(sortButton);

    CPushButton *nextButton = new CPushButton("Next");
    innerBottom->addWidget(nextButton);
}


void CyrusDialog::initializeCreatureRepository(QHBoxLayout* outer){
    
    QVBoxLayout *inner = new QVBoxLayout;
    outer->addLayout(inner);

    // Player Character Repository
    QHBoxLayout *creatureRepositoryBannerLayout = new QHBoxLayout;
    inner->addLayout(creatureRepositoryBannerLayout);
    
    auto banner = new CLabel("Character Repository");
    creatureRepositoryBannerLayout->addWidget(banner);

    auto *addCreatureButton = new CPushButton("+");
    creatureRepositoryBannerLayout->addWidget(addCreatureButton);
    auto *removeCreatureButton = new CPushButton("-");
    creatureRepositoryBannerLayout->addWidget(removeCreatureButton);

    auto creatureRepository = new CTableView();
    auto itemDelegate = new CStyledItemDelegate();
    creatureRepository->setItemDelegate(itemDelegate);
    auto pcList = new CreatureModel();
    creatureRepository->setModel(pcList);
    inner->addWidget(creatureRepository);
    creatureRepository->show();

    //connections
    connect(addCreatureButton, &QPushButton::clicked, [pcList](){
        CreatureItem temp;
        temp.name = "New PC";
        temp.type = "PC";
        temp.level = 1;
        temp.initiative = 0;
        pcList->addRows(temp);
    });

    //connections
    connect(removeCreatureButton, &QPushButton::clicked, [pcList](){
        CreatureItem temp;
        temp.name = "New PC";
        temp.type = "PC";
        temp.level = 1;
        temp.initiative = 0;
        pcList->addRows(temp);
    });

    // Effect Tracker
    /*QHBoxLayout *effectBannerLayout = new QHBoxLayout;
    inner->addLayout(effectBannerLayout);
    effectBannerLayout->addWidget(new QLabel("Effect Tracker"));
    QPushButton *addEffectButton = new QPushButton("Add Effect");
    effectBannerLayout->addWidget(addEffectButton);

    QListView *effectTracker = new QListView();
    auto effectList = new QStringListModel(QStringList() << "Cloudkill" << "Stoneskin" << "Stunned");
    effectList->setHeaderData(0, Qt::Horizontal, "Target");
    effectList->setHeaderData(1, Qt::Horizontal, "Duration Left");
    effectTracker->setModel(effectList);
    effectTracker->setDragEnabled(true);
    inner->addWidget(effectTracker);
    effectTracker->show();*/
}