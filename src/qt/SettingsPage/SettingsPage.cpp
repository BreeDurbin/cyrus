#include "SettingsPage.h"
#include "ui_SettingsPage.h"

SettingsPage::SettingsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsPage)
{
    ui->setupUi(this);
    this->setAutoFillBackground(false);
}

SettingsPage::~SettingsPage()
{
    delete ui;
}


