#include "CyrusComboBox.h"
#include <QHBoxLayout>
#include "ColorRepository.h"
#include "IconRepository.h"
#include "StyleRepository.h"

CyrusComboBox::CyrusComboBox(QWidget* parent)
    : QWidget(parent),
      edit_(new QLineEdit(this)),
      button_(new CyrusToolButton(this)),
      menu_(new QMenu(this))
{
    auto layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(edit_, /*stretch=*/ 1);
    layout->addWidget(button_, /*stretch=*/ 0);

    int height = parentWidget()->height();

    button_->setIcon(IconRepository::down_arrow_icon());
    button_->setFixedSize(height, height);
    button_->setIconSize(QSize(height, height));

    edit_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

    // Apply font to menu
    menu_->setFont(QFont(StyleRepository::textFontFamily()));
    
    connect(menu_, &QMenu::triggered, this, &CyrusComboBox::onActionTriggered);
    connect(edit_, &QLineEdit::textEdited, this, &CyrusComboBox::onTextEdited);

    // align meunu with lineedit 
    connect(button_, &QToolButton::clicked, this, [this]() {
        QPoint menuPos = edit_->mapToGlobal(QPoint(0, edit_->height()));
        menu_->setFixedWidth(edit_->width());
        menu_->exec(menuPos);
    });

    edit_->setStyleSheet(StyleRepository::lineEdit());
}

void CyrusComboBox::addItem(const QString& text, const QVariant& userData)
{
    QAction* action = new QAction(text, this);
    action->setData(userData);
    menu_->addAction(action);
    actions_.append(action);
}

void CyrusComboBox::addItems(const QStringList& items)
{
    for (const QString& item : items)
        addItem(item);
}

QString CyrusComboBox::currentText() const
{
    return edit_->text();
}

QVariant CyrusComboBox::currentData() const
{
    if (currentIndex_ >= 0 && currentIndex_ < actions_.size())
        return actions_[currentIndex_]->data();
    return {};
}

void CyrusComboBox::setText(const QString& text)
{
    edit_->setText(text);
}

void CyrusComboBox::setCurrentIndex(int index){
    currentIndex_ = index;
}

QSize CyrusComboBox::sizeHint() const {
    return QSize(edit_->sizeHint().width() + button_->sizeHint().width(),
                 edit_->sizeHint().height());
}

QSize CyrusComboBox::minimumSizeHint() const {
    return QSize(edit_->minimumSizeHint().width() + button_->minimumSizeHint().width(),
                 edit_->minimumSizeHint().height());
}


void CyrusComboBox::onActionTriggered(QAction* action)
{
    currentIndex_ = actions_.indexOf(action);
    edit_->setText(action->text());
    emit currentIndexChanged(action->text());
    emit currentIndexChanged(currentIndex_);
}

void CyrusComboBox::onTextEdited(const QString& text)
{
    currentIndex_ = -1; // typing overrides selection
    emit textChanged(text);
}
