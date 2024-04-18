#include "HistoryPage.h"
#include "ui_HistoryPage.h"

HistoryPage::HistoryPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HistoryPage)
{
    ui->setupUi(this);
}

HistoryPage::~HistoryPage()
{
    delete ui;
}


