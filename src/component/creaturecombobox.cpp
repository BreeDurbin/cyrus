#include "creaturecombobox.h"
#include <dracula.h>

CreatureComboBox::CreatureComboBox(QWidget *parent)
    : QComboBox(parent)
{
    connect(lineEdit(), &QLineEdit::editingFinished,
            this, [&] () { emit currentTextSaved(currentText()); });
    connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [&] (int index) { Q_UNUSED(index) emit currentTextSaved(currentText()); });

    setStyleSheet();
    
}

