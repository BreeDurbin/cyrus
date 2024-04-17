#include "CampaignPage.h"
#include "ui_CampaignPage.h"

CampaignPage::CampaignPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CampaignPage)
{
    ui->setupUi(this);
}

CampaignPage::~CampaignPage()
{
    delete ui;
}


