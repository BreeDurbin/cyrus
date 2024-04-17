#include "CreaturePage.h"
#include "ui_CreaturePage.h"

CreaturePage::CreaturePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CreaturePage)
{
    ui->setupUi(this);
}

CreaturePage::~CreaturePage()
{
    delete ui;
}


