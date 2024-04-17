#include "include/cpushbutton.h"
#include <QPalette>
#include "../style/include/cstyle.h"

CPushButton::CPushButton(const QString text, QWidget *parent){
    this->setStyle(new CStyle());
    this->setText(text);
    this->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
    this->setFont(QFont("Helvetica", 16, QFont::Bold));
    this->update();
}

CPushButton::~CPushButton(){};