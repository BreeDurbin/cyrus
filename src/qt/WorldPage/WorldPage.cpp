#include "WorldPage.h"
#include "ui_WorldPage.h"

WorldPage::WorldPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WorldPage)
{
    ui->setupUi(this);
}

WorldPage::~WorldPage()
{
    delete ui;
}


