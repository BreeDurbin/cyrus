#include "include/ctableview.h"

#include <QHeaderView>

CTableView::CTableView(const QString text, QWidget *parent){
    this->viewport()->setAutoFillBackground(false);
    this->setAutoFillBackground(false);
    //this->setFrameStyle(QFrame::NoFrame);
    this->horizontalHeader()->hide();
    this->verticalHeader()->hide();
    this->setShowGrid(false);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setViewportMargins(20, 20, 20, 20);
    this->setContentsMargins(20, 20, 20, 20);
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
}

CTableView::~CTableView(){};

