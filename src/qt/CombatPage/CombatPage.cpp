#include "CombatPage.h"
#include "ui_CombatPage.h"

CombatPage::CombatPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CombatPage)
{
    ui->setupUi(this);
}

CombatPage::~CombatPage()
{
    delete ui;
}


