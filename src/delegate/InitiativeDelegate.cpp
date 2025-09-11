#include "InitiativeDelegate.h"

#include <QStyledItemDelegate>
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QCursor>
#include "ColorRepository.h"
#include "StyleRepository.h"
#include "IconRepository.h"
#include "CharacterModel.h"
#include "Character.h"
#include <QAbstractItemView>
#include "Enums.h"
#include "InitiativeView.h"
#include "PainterUtils.h"
#include "InitiativeLayout.h"
#include <QLineEdit>
#include <QPainterPath>

#include<iostream>

InitiativeDelegate::InitiativeDelegate(CharacterController* characterController, QObject *parent) 
    : QStyledItemDelegate(parent), characterController_{characterController} {}


void InitiativeDelegate::paint(QPainter* painter,
                               const QStyleOptionViewItem& option,
                               const QModelIndex& index) const
{
    painter->save();

    const QVariant charVar = index.data(Cyrus::CharacterRole);
    if (!charVar.canConvert<std::shared_ptr<Character>>()) { painter->restore(); return; }
    auto character = charVar.value<std::shared_ptr<Character>>();
    const Character::LayoutSpec& spec = character->layoutSpec();
    const CastState castState = characterController_->stateFor(character->uuid());

    // State checks
    bool isActiveIndex    = checkActiveIndex(index, option);
    bool isCurrentSegment = checkCurrentSegment(index, character->initiative());
    bool isHovered        = checkHovered(option);
    bool isExpanded       = checkExpanded(isActiveIndex, character);
    bool isCharacter      = checkIfCharacter(character);

    QPoint localCursor = cursorPosInItem(option);

    // Use static layout engine
    InitiativeLayout::InitiativeRects rects =
        InitiativeLayout::calculate(option.rect, spec, *character, isActiveIndex, isExpanded);

    // --- Background ---
    QColor bg = isCurrentSegment
        ? ColorRepository::selectedBackground()
        : ColorRepository::baseBackground();

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);
    painter->setBrush(bg);
    painter->drawRoundedRect(rects.baseRect, spec.cornerRadius, spec.cornerRadius);

    // --- Hover outline ---
    if (isHovered || isActiveIndex) {
        QPen hoverPen(ColorRepository::hoverOutlineBrush(rects.baseRect), 1.5);
        painter->setPen(hoverPen);
        painter->setBrush(Qt::NoBrush);
        painter->drawRoundedRect(rects.baseRect.adjusted(-3, -3, 3, 3),
                                 spec.cornerRadius, spec.cornerRadius);
    }

    // --- Hero icon ---
    QIcon heroIcon = character->icon();
    heroIcon.paint(painter, rects.heroIconRect, Qt::AlignCenter,
                                    QIcon::Normal, QIcon::On);

    // --- Initiative number ---
    painter->setFont(StyleRepository::labelFont(14, true));
    painter->setPen(ColorRepository::selectedForeground());
    painter->drawText(rects.initiativeRect, Qt::AlignVCenter | Qt::AlignLeft,
                      QString::number(character->initiative()));

    // --- Name ---
    painter->setFont(StyleRepository::labelFont(14, true));
    painter->setPen(ColorRepository::text());
    painter->drawText(rects.textRect, Qt::AlignVCenter | Qt::AlignLeft, character->text());

    // --- Action icons ---
    // No icon selector unlesss the active index and the active index is a character base class
    if (isActiveIndex && isCharacter) { 
        const QList<QIcon> actionIcons = IconRepository::actionIcons();
        const int iconSize = static_cast<int>(rects.mainRowRect.height() * spec.iconSelectorIconScale);

        for (int i = 0; i < rects.actionIconRects.size(); ++i) {
            const QRect& iconRect = rects.actionIconRects[i];
            bool selected = (static_cast<int>(character->actionType()) == i);
            bool hovered  = iconRect.contains(localCursor);

            PainterUtils::paintActionIcon(painter, actionIcons[i], iconRect, selected, hovered, iconSize);
        }
    }

    // --- Delete button ---
    PainterUtils::paintDeleteButton(painter, rects.deleteRect, rects.deleteRect.contains(localCursor));

    // --- Expanded dropdown ---
    if (isExpanded) {
        switch (character->actionType()) {
        case Cyrus::ActionType::Attack:
            PainterUtils::paintAttackDropdown(painter, rects.dropdownRect, spec);
            break;
        case Cyrus::ActionType::Cast:
            PainterUtils::paintCastDropdown(painter, *character, rects, castState);
            break;
        default:
            break;
        }
    }

    painter->restore();
}

bool InitiativeDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
                                     const QStyleOptionViewItem &option,
                                     const QModelIndex &index)
{
    if (event->type() == QEvent::MouseMove) {
        if (option.widget)
            const_cast<QWidget*>(option.widget)->update(option.rect);
    }

    if (event->type() == QEvent::MouseButtonRelease) {
        const QVariant charVar = index.data(Cyrus::CharacterRole);
        if (!charVar.canConvert<std::shared_ptr<Character>>()) { return false; }
        auto character = charVar.value<std::shared_ptr<Character>>();

        bool isActiveIndex = checkActiveIndex(index, option);
        bool isExpanded    = checkExpanded(isActiveIndex, character);
        QPoint localCursor = cursorPosInItem(option, event);

        InitiativeLayout::InitiativeRects rects =
            InitiativeLayout::calculate(option.rect, character->layoutSpec(),
                                        *character, isActiveIndex, isExpanded);
        InitiativeLayout::HitTestResult hit =
            InitiativeLayout::hitTest(rects, *character, localCursor);

        QUuid id = character->uuid();

        auto clamp = [](int v, int lo, int hi){ return std::max(lo, std::min(hi, v)); };

        switch (hit.type) {
            case InitiativeLayout::HitTestResult::DeleteButton:
                qDebug() << "Delete button clicked for index " << index.row();
                emit deleteItemClicked(index);
                if (option.widget) const_cast<QWidget*>(option.widget)->update(option.rect);
                return true;

            case InitiativeLayout::HitTestResult::ActionIcon:
                //send event to the controller to update the model
                emit iconSelectorClicked(index, static_cast<Cyrus::ActionType>(hit.actionIconIndex));
                if (option.widget) const_cast<QWidget*>(option.widget)->update(option.rect);
                return true;

            case InitiativeLayout::HitTestResult::SpellEdit: {
                if (auto* view = qobject_cast<InitiativeView*>(const_cast<QWidget*>(option.widget))) {
                    qDebug() << "Editing spell at row" << index.row();
                    view->editCustom(index);
                }
                if (option.widget) const_cast<QWidget*>(option.widget)->update(option.rect);
                return true;
            }
            case InitiativeLayout::HitTestResult::CastingTimeMinus:
                emit decrementCastingTimeClicked(id);
                if (option.widget) const_cast<QWidget*>(option.widget)->update(option.rect);
                return true;

            case InitiativeLayout::HitTestResult::CastingTimePlus:
                emit incrementCastingTimeClicked(id);
                if (option.widget) const_cast<QWidget*>(option.widget)->update(option.rect);
                return true;

            case InitiativeLayout::HitTestResult::DurationMinus:
                emit decrementDurationClicked(id);
                if (option.widget) const_cast<QWidget*>(option.widget)->update(option.rect);
                return true;

            case InitiativeLayout::HitTestResult::DurationPlus:
                emit incrementDurationClicked(id);
                if (option.widget) const_cast<QWidget*>(option.widget)->update(option.rect);
                return true;

            case InitiativeLayout::HitTestResult::CastOk:
                emit castSubmitted(id, *character);
                emit iconSelectorClicked(index, Cyrus::ActionType::None);
                if (option.widget) const_cast<QWidget*>(option.widget)->update(option.rect);
                return true;

            case InitiativeLayout::HitTestResult::None:
            default:
                break;
        }
    }

    return false;
}

QWidget* InitiativeDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex& index) const {
    qDebug() << "createEditor";
    
    if (index.data(Cyrus::ModelRoles::RoleTypeRole).toInt() != Cyrus::SpellNameEditRole)
        return nullptr;

    //in this editor I set the stylesheet but a lot of the styling isnt applied
    auto* editor = new QLineEdit(parent);
    editor->setStyleSheet(StyleRepository::lineEdit());
    editor->setAutoFillBackground(true); // Make the lineedit paint its own background

    // Commit on Enter
    connect(editor, &QLineEdit::returnPressed, this, [this, editor]() {
        emit commitData(editor);
        emit closeEditor(editor);
    });

    // Commit on focus loss
    connect(editor, &QLineEdit::editingFinished, this, [this, editor]() {
        emit commitData(editor);
        emit closeEditor(editor);
    });

    return editor;
}

void InitiativeDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
    qDebug() << "setEditorData";
    auto* lineEdit = qobject_cast<QLineEdit*>(editor);
    if (!lineEdit) return;

    auto charVar = index.data(Cyrus::CharacterRole);
    if (!charVar.canConvert<std::shared_ptr<Character>>()) return;

    auto character = charVar.value<std::shared_ptr<Character>>();
    auto state = characterController_->stateFor(character->uuid());
    lineEdit->setPlaceholderText(state.defaultText);
    lineEdit->setText(state.spellName);
}

void InitiativeDelegate::setModelData(QWidget* editor, QAbstractItemModel*, const QModelIndex& index) const {
    qDebug() << "setModelData";
    auto* lineEdit = qobject_cast<QLineEdit*>(editor);
    if (!lineEdit) return;

    //get character payload
    auto charVar = index.data(Cyrus::CharacterRole);
    if (!charVar.canConvert<std::shared_ptr<Character>>()) return;
    auto character = charVar.value<std::shared_ptr<Character>>();

    //get new name
    QString newName = lineEdit->text();

    //notify character controller to update cast state
    emit spellNameEdited(character->uuid(), newName);
}

void InitiativeDelegate::updateEditorGeometry(QWidget* editor,
                                              const QStyleOptionViewItem& option,
                                              const QModelIndex& index) const {

    // get char payload
    const QVariant charVar = index.data(Cyrus::CharacterRole);
    if (!charVar.canConvert<std::shared_ptr<Character>>()) { return; }
    auto character = charVar.value<std::shared_ptr<Character>>();

    // populate bools
    bool isActiveIndex = checkActiveIndex(index, option);
    bool isExpanded    = checkExpanded(isActiveIndex, character);

    // calculate rects
    InitiativeLayout::InitiativeRects rects = InitiativeLayout::calculate(option.rect, character->layoutSpec(),
            *character, isActiveIndex, isExpanded);

    // Use your layout rect for the spell edit box
    editor->setGeometry(rects.cast.spellEdit);
}


QSize InitiativeDelegate::sizeHint(const QStyleOptionViewItem& option,
                                   const QModelIndex& index) const {

    std::shared_ptr<Character> character;
    const QVariant charVar = index.data(Cyrus::CharacterRole);
    if (!charVar.canConvert<std::shared_ptr<Character>>()) { 
        //Mock character for size hint when the list is empty
        character = std::make_shared<Character>("SIZE_HINT_DEFAULT", 10, Cyrus::CharacterType::Fighter, Cyrus::ActionType::Cast);
    }
    character = charVar.value<std::shared_ptr<Character>>();
    const Character::LayoutSpec spec = character->layoutSpec();

    const int width     = InitiativeLayout::preferredWidth(spec, character);
    bool isActiveIndex = checkActiveIndex(index, option);
    bool isExpanded    = checkExpanded(isActiveIndex, character);

    int height = isExpanded ? 2 * spec.preferredHeight : spec.preferredHeight;
    return QSize(width, height);
}


bool InitiativeDelegate::checkActiveIndex(const QModelIndex& index, const QStyleOptionViewItem& option) const {
    auto v = qobject_cast<const InitiativeView*>(option.widget);
    bool result = v && (v->activeIndex() == index);
    //qDebug() << "checkActiveIndex: row" << index.row() << "active?" << result;
    return result;
}

bool InitiativeDelegate::checkExpanded(bool isActiveIndex, std::shared_ptr<Character> character) const {
    //Only expand for characters(base class) not actions
    if (character && typeid(*character) == typeid(Character)) {
        return isActiveIndex && character->actionType() != Cyrus::ActionType::None;
    }
    return false;
}

bool InitiativeDelegate::checkCurrentSegment(const QModelIndex& index, const int currentInitiative) const {
    const QVariant segmentVar = index.data(Cyrus::ModelRoles::SegmentRole);
    const int currentSegment = !segmentVar.canConvert<int>() ? 0 : segmentVar.value<int>();
    return currentInitiative == currentSegment;
}

bool InitiativeDelegate::checkHovered(const QStyleOptionViewItem &option) const {
    return (option.state & QStyle::State_MouseOver);
}

bool InitiativeDelegate::checkIfCharacter(std::shared_ptr<Character> character) const {
    return character && typeid(*character) == typeid(Character);
}

QPoint InitiativeDelegate::cursorPosInItem(const QStyleOptionViewItem& option, const QEvent* event) const {
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
