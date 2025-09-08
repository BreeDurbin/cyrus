#include "CombatLogDelegate.h"
#include "Character.h"

#include <QStyledItemDelegate>
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QCursor>
#include "../style/ColorRepository.h"
#include "../style/StyleRepository.h"
#include "CharacterModel.h"
#include "../helper/RectBuilder.h"

#include<iostream>

using namespace RectBuilder;

CombatLogDelegate::CombatLogDelegate(QObject *parent)
    : QStyledItemDelegate(parent) {}

void CombatLogDelegate::paint(QPainter* painter,
                              const QStyleOptionViewItem& option,
                              const QModelIndex& index) const
{
    painter->save();

    // Get Character payload
    const QVariant var = index.data(CharacterModel::CharacterRole);
    if (!var.canConvert<Character>()) {
        painter->restore();
        return;
    }
    const Character character = var.value<Character>();
    const auto& layoutSpec = character.layoutSpec();

    // Layout rects
    const QRect rect       = padRectangle(option.rect, layoutSpec.padding);
    const QRect iconRect   = buildHeroIconRect(rect, layoutSpec);
    int spacing = layoutSpec.padding;
    const QRect textRect(iconRect.right() + spacing,
                        rect.top(),
                        rect.right() - (iconRect.right() + 2 * spacing),
                        rect.height());

    //setup background color to be painted
    QColor bg = ColorRepository::baseBackground();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);
    painter->setBrush(bg);
    painter->drawRoundedRect(rect, 12, 12);

    // Optional hover outline using the repo’s gradient brush
    const bool hovered  = (option.state & QStyle::State_MouseOver);
    if (hovered) {
        QPen hoverPen(ColorRepository::hoverOutlineBrush(rect), 1.5);
        painter->setPen(hoverPen);
        painter->setBrush(Qt::NoBrush);
        painter->drawRoundedRect(rect.adjusted(0.5, 0.5, -0.5, -0.5), 12, 12);
    }

    // Icon
    character.characterIcon().paint(painter, iconRect, Qt::AlignCenter,
                                    QIcon::Normal, QIcon::On);

    // Name (uses text() from repo; on selection background it remains readable)
    painter->setFont(StyleRepository::labelFont(12, false));
    painter->setPen(ColorRepository::text());
    painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, character.combatLog());

    painter->restore();
}


QSize CombatLogDelegate::sizeHint(const QStyleOptionViewItem& option,
                                  const QModelIndex& index) const
{
    const QVariant var = index.data(CharacterModel::CharacterRole);
    if (!var.canConvert<Character>()) {
        return QStyledItemDelegate::sizeHint(option, index);
    }

    const Character character = var.value<Character>();
    const Character::LayoutSpec spec = character.layoutSpec();
    const int width = option.rect.width() > 0 ? option.rect.width() : 200;
    return QSize(width, spec.preferredHeight);
}

