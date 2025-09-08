#include "InitiativeDelegate.h"

#include <QStyledItemDelegate>
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QCursor>
#include "style/ColorRepository.h"
#include "style/StyleRepository.h"
#include "CharacterModel.h"
#include "Character.h"
#include <QAbstractItemView>
#include "shared/Enums.h"
#include "view/InitiativeView.h"
#include "PainterUtils.h"

#include<iostream>

using namespace RectBuilder;

InitiativeDelegate::InitiativeDelegate(QObject *parent)
    : QStyledItemDelegate(parent) {}

void InitiativeDelegate::paint(QPainter* painter,
                               const QStyleOptionViewItem& option,
                               const QModelIndex& index) const
{
    painter->save();

    const QVariant charVar = index.data(CharacterModel::CharacterRole);
    if (!charVar.canConvert<Character>()) {
        painter->restore();
        return;
    }
    const Character character = charVar.value<Character>();
    const LayoutSpec& spec = character.layoutSpec();

    // State checks
    bool isActiveIndex   = checkActiveIndex(option, index);
    bool isCurrentSegment = checkCurrentSegment(index, character.initiative());
    bool isHovered        = checkHovered(option);
    bool isExpanded       = checkExpanded(isActiveIndex, character.actionType());

    QPoint localCursor = cursorPosInItem(option);

    // 🔑 Use layout engine instead of calculateRects
    InitiativeLayout layout(spec, character, isActiveIndex, isExpanded);
    InitiativeRects rects = layout.calculate(option.rect);

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
    character.characterIcon().paint(painter, rects.heroIconRect, Qt::AlignCenter,
                                    QIcon::Normal, QIcon::On);

    // --- Initiative number ---
    painter->setFont(StyleRepository::labelFont(14, true));
    painter->setPen(ColorRepository::selectedForeground());
    painter->drawText(rects.initiativeRect, Qt::AlignVCenter | Qt::AlignLeft,
                      QString::number(character.initiative()));

    // --- Name ---
    painter->setFont(StyleRepository::labelFont(14, true));
    painter->setPen(ColorRepository::text());
    painter->drawText(rects.textRect, Qt::AlignVCenter | Qt::AlignLeft, character.name());

    // --- Action icons ---
    if (isActiveIndex) {
        const QList<QIcon> actionIcons = IconRepository::actionIcons();
        const int iconSize = static_cast<int>(rects.mainRowRect.height() * spec.iconSelectorIconScale);

        for (int i = 0; i < rects.actionIconRects.size(); ++i) {
            const QRect& iconRect = rects.actionIconRects[i];
            bool selected = (static_cast<int>(character.actionType()) == i);
            bool hovered  = iconRect.contains(localCursor);

            PainterUtils::drawActionIcon(painter, actionIcons[i], iconRect, selected, hovered, iconSize);
        }

    }

    // --- Delete button ---
    PainterUtils::drawDeleteButton(painter, rects.deleteRect, rects.deleteRect.contains(localCursor));

    // --- Expanded dropdown ---
    if (isExpanded) {
        switch (character.actionType()) {
        case Cyrus::ActionType::Attack:
            PainterUtils::drawAttackDropdown(painter, rects.dropdownRect, spec);
            break;
        case Cyrus::ActionType::Cast:
            PainterUtils::drawCastDropdown(painter, character, rects);
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
        const QVariant var = index.data(CharacterModel::CharacterRole);
        if (!var.canConvert<Character>()) return true;
        Character character = var.value<Character>();

        bool isActiveIndex = checkActiveIndex(option, index);
        bool isExpanded    = checkExpanded(isActiveIndex, character.actionType());

        InitiativeRects rects = LayoutEngine::calculate(option.rect, character,
                                                        isActiveIndex, isExpanded);

        QPoint localCursor = cursorPosInItem(option, event);
        HitTestResult hit = LayoutEngine::hitTest(rects, character, localCursor);

        QUuid id = character.uuid();
        CastState castState = castState_[id];

        auto clamp = [](int v, int lo, int hi){ return std::max(lo, std::min(hi, v)); };

        switch (hit.type) {
        case HitTestResult::DeleteButton:
            emit deleteItem(index);
            return true;

        case HitTestResult::ActionIcon:
            character.setActionType(static_cast<Cyrus::ActionType>(hit.actionIconIndex));
            static_cast<CharacterModel*>(model)->setItem(index, character);
            return true;

        case HitTestResult::SpellEdit:
            emit requestSpellNameEdit(index, castState.spellName);
            return true;

        case HitTestResult::CastingTimeMinus:
            castState.castingTime = clamp(castState.castingTime - 1, 0, 10);
            castState_[id] = castState;
            if (option.widget) option.widget->update(option.rect);
            return true;

        case HitTestResult::CastingTimePlus:
            castState.castingTime = clamp(castState.castingTime + 1, 0, 10);
            castState_[id] = castState;
            if (option.widget) option.widget->update(option.rect);
            return true;

        case HitTestResult::DurationMinus:
            castState.duration = clamp(castState.duration - 1, 0, 99);
            castState_[id] = castState;
            if (option.widget) option.widget->update(option.rect);
            return true;

        case HitTestResult::DurationPlus:
            castState.duration = clamp(castState.duration + 1, 0, 99);
            castState_[id] = castState;
            if (option.widget) option.widget->update(option.rect);
            return true;

        case HitTestResult::CastOk:
            emit castSubmitted(index, castState);
            if (option.widget) option.widget->update(option.rect);
            return true;

        case HitTestResult::None:
        default:
            break;
        }
    }

    return false;
}


QSize InitiativeDelegate::sizeHint(const QStyleOptionViewItem& option,
                                   const QModelIndex& index) const {
    const QVariant var = index.data(CharacterModel::CharacterRole);
    if (!var.canConvert<Character>()) {
        return QStyledItemDelegate::sizeHint(option, index);
    }
    const Character character = var.value<Character>();
    const Character::LayoutSpec spec = character.layoutSpec();
    const int width     = option.rect.width() > 0 ? option.rect.width() : 300;
    bool isActiveIndex = checkActiveIndex(option, index);
    bool isExpanded    = checkExpanded(isActiveIndex, character.actionType());

    int height = isExpanded ? 2 * spec.preferredHeight : spec.preferredHeight;
    return QSize(width, height);
}

bool InitiativeDelegate::checkActiveIndex(const QStyleOptionViewItem& option,
                                          const QModelIndex& index) const {
    auto v = qobject_cast<const InitiativeView*>(option.widget);
    bool result = v && (v->activeIndex() == index);
    qDebug() << "checkActiveIndex: row" << index.row() << "active?" << result;
    return result;
}

bool InitiativeDelegate::checkExpanded(bool isActiveIndex,
                                       Cyrus::ActionType actionType) const {
    return isActiveIndex && actionType != Cyrus::ActionType::None;
}

bool InitiativeDelegate::checkCurrentSegment(const QModelIndex& index,
                                             const int currentInitiative) const {
    const QVariant segmentVar = index.data(CharacterModel::SegmentRole);
    const int currentSegment = !segmentVar.canConvert<int>() ? 0 : segmentVar.value<int>();
    return currentInitiative == currentSegment;
}

bool InitiativeDelegate::checkHovered(const QStyleOptionViewItem &option) const {
    return (option.state & QStyle::State_MouseOver);
}

QPoint InitiativeDelegate::cursorPosInItem(const QStyleOptionViewItem& option,
                                           const QEvent* event) const
{
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
