#include "SentinelEditor.h"

#include "StyleRepository.h"
#include "IconRepository.h"
#include <QMetaObject>

#include "SentinelEditor.h"
#include "StyleRepository.h"
#include "IconRepository.h"
#include <QHBoxLayout>
#include <QMetaObject>

SentinelEditor::SentinelEditor(QWidget* parent)
    : QWidget(parent)
    , lineEdit_(new QLineEdit(this))
    , addButton_(new QPushButton(this))
{
    setAttribute(Qt::WA_NoSystemBackground);
    setFocusPolicy(Qt::StrongFocus);

    // --- Line edit setup ---
    lineEdit_->setStyleSheet(StyleRepository::lineEdit());
    lineEdit_->setFrame(false);
    lineEdit_->setTextMargins(0, 0, 0, 0);
    lineEdit_->setPlaceholderText("Add a character...");
    lineEdit_->setFocusPolicy(Qt::StrongFocus);
    lineEdit_->selectAll();
    QMetaObject::invokeMethod(lineEdit_, "setFocus", Qt::QueuedConnection);

    // --- Button setup ---
    addButton_->setStyleSheet(StyleRepository::iconPushButton());
    addButton_->setFlat(true);
    addButton_->setContentsMargins(0, 0, 0, 0);
    addButton_->setIcon(IconRepository::plus_icon());

    // --- Horizontal layout ---
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0); 
    layout->setSpacing(4);                    
    layout->addWidget(lineEdit_);
    layout->addWidget(addButton_);
    
    setLayout(layout);
}

