#include "include/cpushbutton.hpp"
#include <QPalette>

CPushButton::CPushButton(const QString text, QWidget *parent){
    this->setText(text);
    this->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
    this->setFont(QFont("Helvetica", 16, QFont::Bold));

    QPalette pal = this->palette();
    pal.setColor(QPalette::Button, QColor(12, 51, 194));
    pal.setColor(QPalette::ButtonText, QColor(235, 180, 15));
    pal.setColor(QPalette::Highlight, QColor(235, 180, 15));
    this->setAutoFillBackground(true);
    this->setPalette(pal);
    this->update();
}

CPushButton::~CPushButton(){};