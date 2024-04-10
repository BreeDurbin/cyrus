#include "include/ctableview.hpp"

#include <QHeaderView>

CTableView::CTableView(const QString text, QWidget *parent){
    this->viewport()->setAutoFillBackground(false);
    this->setAutoFillBackground(false);
    this->setFrameStyle(QFrame::NoFrame);
    this->horizontalHeader()->hide();
    this->verticalHeader()->hide();
    this->setShowGrid(false);
    this->setColumnWidth(0, 200);

}

CTableView::~CTableView(){};

