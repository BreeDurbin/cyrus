#include "include/clabel.h"

CLabel::CLabel(const QString text, QWidget *parent){
    this->setText(text);
    this->setFont(QFont("Helvetica", 24, QFont::Bold));
    auto palette = this->palette();
    palette.setColor(this->foregroundRole(), QColor(235, 180, 15));
    this->setPalette(palette);
}

CLabel::~CLabel(){}