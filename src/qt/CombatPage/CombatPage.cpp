#include "CombatPage.h"
#include "ui_CombatPage.h"

CombatPage::CombatPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CombatPage)
{
    ui->setupUi(this);
    ui->encounterListView->viewport()->setAutoFillBackground(false);
    ui->initiativeListView->viewport()->setAutoFillBackground(false);
}

CombatPage::~CombatPage()
{
    delete ui;
}


