#include "RosterDelegate.h"

#include <QStyledItemDelegate>
#include <QPainter>
#include <QMouseEvent>
#include <QCursor>
#include "ColorRepository.h"
#include "StyleRepository.h"
#include "IconRepository.h"
#include "CharacterModel.h"
#include "Character.h"
#include <QAbstractItemView>
#include "Enums.h"
#include "PainterUtils.h"
#include <QLineEdit>
#include <QPainterPath>
#include "LayoutUtils.h"
#include "LayoutFactory.h"
#include "RosterLayoutEngine.h"

#include<iostream>

RosterDelegate::RosterDelegate(CharacterController* characterController, QObject *parent) 
    : QStyledItemDelegate(parent), characterController_{characterController} {
        auto* engine = LayoutFactory::rosterLayoutEngine();
        connect(engine, &RosterLayoutEngine::incrementRosterMemberInitiativeClicked,
                this,   &RosterDelegate::incrementRosterMemberInitiativeClicked);
        connect(engine, &RosterLayoutEngine::decrementRosterMemberInitiativeClicked,
                this,   &RosterDelegate::decrementRosterMemberInitiativeClicked);
        connect(engine, &RosterLayoutEngine::cloneRosterMemberClicked,
                this,   &RosterDelegate::cloneRosterMemberClicked);
        connect(engine, &RosterLayoutEngine::deleteButtonClicked,
                this,   &RosterDelegate::deleteButtonClicked);
}

void RosterDelegate::paint(QPainter* painter,
                           const QStyleOptionViewItem& option,
                           const QModelIndex& index) const 
{
    painter->save();

    // --- Character payload ---
    const QVariant charVar = index.data(Cyrus::CharacterRole);
    if (!charVar.canConvert<std::shared_ptr<Character>>()) {
        painter->restore();
        return;
    }
    auto character = charVar.value<std::shared_ptr<Character>>();
    const auto& spec = character->layoutSpec();

    // --- Get layout engine ---
    auto* engine = LayoutFactory::rosterLayoutEngine();

    // --- Cursor position in local coords ---
    QPoint localCursor = cursorPosInItem(option);

    // --- Calculate layout ---
    auto layout = engine->calculate(option.rect, character);

    // --- Background ---
    const bool isHovered      = option.state & QStyle::State_MouseOver;
    const bool isCurrentIndex = option.state & QStyle::State_Selected;

    QColor bg = isCurrentIndex
        ? ColorRepository::selectedBackground()
        : ColorRepository::baseBackground();

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);
    painter->setBrush(bg);
    painter->drawRoundedRect(layout->baseRect, spec.cornerRadius, spec.cornerRadius);

    // --- Hover/active outline ---
    if (isHovered || isCurrentIndex) {
        QPen hoverPen(ColorRepository::hoverOutlineBrush(layout->baseRect), 1.5);
        painter->setPen(hoverPen);
        painter->setBrush(Qt::NoBrush);
        painter->drawRoundedRect(layout->baseRect.adjusted(-3, -3, 3, 3),
                                 spec.cornerRadius, spec.cornerRadius);
    }

    // --- Paint the actual layout contents ---
    engine->paintLayout(painter, layout, character, CastState(), isCurrentIndex, false, localCursor);

    painter->restore();
}

bool RosterDelegate::editorEvent(QEvent *event,
                                 QAbstractItemModel *model,
                                 const QStyleOptionViewItem &option,
                                 const QModelIndex &index)
{
    if (event->type() == QEvent::MouseMove) {
        if (option.widget)
            const_cast<QWidget*>(option.widget)->update(option.rect);
    }

    if (event->type() == QEvent::MouseButtonRelease) {

        const QWidget* viewport = option.widget;
        auto* view = qobject_cast<QListView*>(viewport->parentWidget());

        // --- Character payload ---
        const QVariant charVar = index.data(Cyrus::CharacterRole);
        if (!charVar.canConvert<std::shared_ptr<Character>>()) {
            return false;
        }
        auto character = charVar.value<std::shared_ptr<Character>>();
        auto* engine   = LayoutFactory::rosterLayoutEngine();

        // --- Layout ---
        auto layout = engine->calculate(option.rect, character);

        // --- Cursor ---
        QPoint localCursor = cursorPosInItem(option, event);

        auto callback = engine->hitTest(index, layout, character, localCursor);
        if(callback){
            callback->execute(view);
            return true;
        }
    }

    return false;
}

QSize RosterDelegate::sizeHint(const QStyleOptionViewItem& option,
                                   const QModelIndex& index) const {

    std::shared_ptr<Character> character;
    const QVariant charVar = index.data(Cyrus::CharacterRole);
    if (!charVar.canConvert<std::shared_ptr<Character>>()) { 
        //Mock character for size hint when the list is empty
        character = std::make_shared<Character>("SIZE_HINT_DEFAULT", 10, Cyrus::CharacterType::Fighter, Cyrus::ActionType::Cast);
    }
    character = charVar.value<std::shared_ptr<Character>>();

    const int width     = minimumWidth(*character);
    const int height = character->layoutSpec().preferredHeight;

    return QSize(width, height);
}

QPoint RosterDelegate::cursorPosInItem(const QStyleOptionViewItem& option, const QEvent* event) const {
    if (!option.widget) {
        return QPoint(-9999, -9999);
    }
    if (event) {
        if (const auto* mouseEvent = dynamic_cast<const QMouseEvent*>(event)) {
            return mouseEvent->pos();
        }
    }
    return option.widget->mapFromGlobal(QCursor::pos());
}

int RosterDelegate::minimumWidth(const Character& character) const {
    // calculate the mimimum width of the rows
    return 400;
}