#include <creaturecombobox.hpp>

CreatureComboBox::CreatureComboBox(QWidget *parent)
    : CComboBox(parent)
{
    connect(lineEdit(), &QLineEdit::editingFinished,
            this, [&] () { emit currentTextSaved(currentText()); });
    connect(this, QOverload<int>::of(&CComboBox::currentIndexChanged),
            this, [&] (int index) { Q_UNUSED(index) emit currentTextSaved(currentText()); });
    
}

