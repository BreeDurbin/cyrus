#include "CyrusDialog.h"
#include "ui_CyrusDialog.h"

#include "../Database/Database.h"
#include "../CombatPage/CombatPage.h"
#include "../CampaignPage/CampaignPage.h"
#include "../CreaturePage/CreaturePage.h"
#include "../SettingsPage/SettingsPage.h"
#include "../WorldPage/WorldPage.h"
#include "../HistoryPage/HistoryPage.h"

#include <iostream>

#include <QStringList>
#include <QGroupBox>
#include <QTextEdit>
#include <QStringListModel>




CyrusDialog::CyrusDialog(QWidget *parent) : QMainWindow(parent), ui(new Ui::CyrusDialog)
{
    Q_INIT_RESOURCE(icon);
    Q_INIT_RESOURCE(image);

    ui->setupUi(this);

    //title and size
    this->setWindowTitle("Cyrus");

    setStyleSheet("#CyrusDialog { border-image: url(:/image/background4.png) 0 0 0 0 stretch fixed;}");

    //database setup
    Database *db = new Database(this);

    //tab setup
    addCombatPage();
    addCreaturePage();
    addWorldPage();
    addHistoryPage();
    addCampaignPage();
    addSettingsPage();
    
}

CyrusDialog::~CyrusDialog()
{
    Q_CLEANUP_RESOURCE(icon);
    Q_CLEANUP_RESOURCE(image);
    delete ui;
}


void CyrusDialog::addCombatPage()
{
    auto combatPage = new CombatPage();
    combatPage->setAutoFillBackground(false);
    ui->tabWidget->addTab(combatPage, "Combat");
}


void CyrusDialog::addCreaturePage()
{
    ui->tabWidget->addTab(new CreaturePage(), "Creature");
}


void CyrusDialog::addWorldPage()
{
    ui->tabWidget->addTab(new WorldPage(), "World");
}

void CyrusDialog::addHistoryPage()
{
    ui->tabWidget->addTab(new HistoryPage(), "History");
}

void CyrusDialog::addCampaignPage()
{
    ui->tabWidget->addTab(new CampaignPage(), "Campaign");
}


void CyrusDialog::addSettingsPage()
{
    ui->tabWidget->addTab(new SettingsPage(), "Settings");
}