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
#include <QHBoxLayout>
#include <QPushButton>
#include "SentinelEditor.h"

#include<iostream>

RosterDelegate::RosterDelegate(CharacterController* characterController, QAbstractItemView* view, QObject *parent) 
    : QStyledItemDelegate(parent), 
    characterController_{characterController},
    view_{view}  {};

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
    auto layout = engine->calculate(option.rect, character, false, false);

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
    // no hover border effect on sentinel cell
    if (index.row() < index.model()->rowCount() - 1 && (isHovered || isCurrentIndex)) {
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

        // --- Character payload ---
        const QVariant charVar = index.data(Cyrus::CharacterRole);
        if (!charVar.canConvert<std::shared_ptr<Character>>()) {
            return false;
        }
        auto character = charVar.value<std::shared_ptr<Character>>();
        auto* engine   = LayoutFactory::rosterLayoutEngine();

        // --- Layout ---
        auto layout = engine->calculate(option.rect, character, false, false, Cyrus::CombatSequenceState::NPC_DETERMINATION);

        // --- Cursor ---
        QPoint localCursor = cursorPosInItem(option, event);

        auto callback = engine->hitTest(index, layout, character, localCursor);
        if(callback){
            callback->execute(this);
            return true;
        }
    }

    return false;
}

QWidget* RosterDelegate::createEditor(QWidget* parent,
                                      const QStyleOptionViewItem& option,
                                      const QModelIndex& index) const
{
    auto* model = index.model();
    int sentinelRow = model->rowCount() - 1;

    if (!index.isValid()) {
        qDebug() << " -> Index invalid! Aborting editor creation.";
        return nullptr;
    }

    if (index.row() != sentinelRow) {
        qDebug() << " -> Not sentinel row, using default editor.";
        return QStyledItemDelegate::createEditor(parent, option, index);
    }

    auto* engine = LayoutFactory::rosterLayoutEngine();
    auto handle = engine->createEditorHandle(parent);

    if (!handle.widget) {
        return nullptr;
    }

    if (handle.command.has_value()) {
        handle.command->execute(const_cast<RosterDelegate*>(this));
    }

    return handle.widget;
}

void RosterDelegate::updateEditorGeometry(QWidget* editor,
                                          const QStyleOptionViewItem& option,
                                          const QModelIndex& index) const
{
    // --- Character payload ---
    const QVariant charVar = index.data(Cyrus::CharacterRole);
    if (!charVar.canConvert<std::shared_ptr<Character>>()) {
        return;
    }
    auto character = charVar.value<std::shared_ptr<Character>>();

    if (character->isSentinel()) {

        const auto& spec = character->layoutSpec();

        // calculate rects
        auto* engine = LayoutFactory::rosterLayoutEngine();
        auto layout = engine->calculate(option.rect, character, false, false);
        auto sentinelLayout = std::dynamic_pointer_cast<SentinelLayout>(layout);
        if (!sentinelLayout) return;

        qDebug() << "updateEditorGeometry()";
        qDebug() << "  option.rect:" << option.rect;
        qDebug() << "  mainRowRect:" << sentinelLayout->mainRowRect;
        qDebug() << "  lineEditFrame:" << sentinelLayout->lineEditFrame;
        qDebug() << "  buttonFrame:" << sentinelLayout->buttonFrame;

        editor->setGeometry(sentinelLayout->mainRowRect);

        // Now position children relative to editor
        if (auto* container = qobject_cast<SentinelEditor*>(editor)) {
            // line edit
            qDebug() << "  setting lineEdit geometry to" << sentinelLayout->lineEditFrame;
            auto lineEdit = container->lineEdit();
            lineEdit->setGeometry(sentinelLayout->lineEditFrame);

            // button
            auto button = container->addButton();
            qDebug() << "  setting button geometry to" << sentinelLayout->buttonFrame;
            button->setGeometry(sentinelLayout->buttonFrame);
            button->setIconSize(button->size());
        }
    }
}



QSize RosterDelegate::sizeHint(const QStyleOptionViewItem& option,
                               const QModelIndex& index) const 
{
    std::shared_ptr<Character> character;

    const QVariant charVar = index.data(Cyrus::CharacterRole);
    if (charVar.isValid() && charVar.canConvert<std::shared_ptr<Character>>()) {
        character = charVar.value<std::shared_ptr<Character>>();
    } else {
        // Mock character for sentinel or empty case
        character = std::make_shared<Character>(
            "SIZE_HINT_DEFAULT",
            10, 0, 0,
            Cyrus::CharacterType::Fighter,
            Cyrus::ActionType::Cast
        );
    }

    const int width  = minimumWidth(*character);
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
    RosterLayoutEngine engine;
    return engine.minimumWidth(std::make_shared<Character>(character));
}