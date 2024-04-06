#include "include/ccombobox.hpp"

CComboBox::CComboBox(QWidget *parent) : QComboBox(parent)
{
    setStyleSheet("QComboBox { background-color: rgb(235, 180, 15); color: rgb(0, 0, 0); }");
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
}

CComboBox::~CComboBox()
{
}