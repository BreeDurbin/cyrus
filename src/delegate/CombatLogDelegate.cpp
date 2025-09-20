#include "CombatLogDelegate.h"

#include "StyleRepository.h"
#include "LayoutUtils.h"
#include "PainterUtils.h"
#include "Character.h"
#include "CharacterModel.h"
#include "IconRepository.h"
#include "ColorRepository.h"

#include <QPainter>
#include <QMouseEvent>
#include "Enums.h"

CombatLogDelegate::CombatLogDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

// ----------------- Painting -----------------
// ----------------- Painting -----------------
void CombatLogDelegate::paint(QPainter* painter,
                              const QStyleOptionViewItem& option,
                              const QModelIndex& index) const
{
    painter->save();

    const QVariant charVar = index.data(Cyrus::CharacterRole);
    if (!charVar.canConvert<std::shared_ptr<Character>>()) {
        qDebug() << "Combat Log: Index data cannot convert to Character";
        painter->restore();
        return;
    }
    auto character = charVar.value<std::shared_ptr<Character>>();

    const LayoutSpec& spec = character->layoutSpec();
    const int padding = spec.padding;

    // Base rect
    QRect baseRect = LayoutUtils::padRectangle(option.rect, spec.padding);

    LayoutUtils::LeftAnchorBuilder left(baseRect);

    // Hero icon
    QRect heroIconRect = left.icon(spec.heroIconScale, padding);

    // Text rect
    int textWidth = baseRect.width() - (heroIconRect.right() - baseRect.left()) - 2 * padding;
    QRect textRect = left.text(textWidth, padding);

    // Right boundary (if needed)
    QRect rightBoundary(baseRect.right(), baseRect.top(), 0, baseRect.height());

    // Background
    painter->setBrush(option.state & QStyle::State_Selected
                      ? ColorRepository::selectedBackground()
                      : ColorRepository::baseBackground());
    painter->setPen(Qt::NoPen);
    painter->drawRect(baseRect);

    // Hero icon painting
    const QIcon heroIcon = IconRepository::iconFor(character->characterType());
    if (!heroIcon.isNull()) {
        heroIcon.paint(painter, heroIconRect, Qt::AlignCenter, QIcon::Normal, QIcon::On);
    } else {
        qDebug() << "Combat log: Hero icon is null";
    }

    // Text painting
    painter->setPen(ColorRepository::text());
    painter->drawText(textRect,
                      Qt::AlignVCenter | Qt::AlignLeft | Qt::TextWordWrap,
                      character->combatLog());

    painter->restore();
}


QSize CombatLogDelegate::sizeHint(const QStyleOptionViewItem& option,
                                  const QModelIndex& index) const
{
    //Character payload
    const QVariant charVar = index.data(Cyrus::CharacterRole);
    if (!charVar.canConvert<std::shared_ptr<Character>>()) {
        return QStyledItemDelegate::sizeHint(option, index);
    }

    auto character = charVar.value<std::shared_ptr<Character>>();
    const LayoutSpec& spec = character->layoutSpec();
    const int padding = spec.padding;

    // Start with padded base rect
    QRect baseRect = LayoutUtils::padRectangle(option.rect, spec.padding);

    // base Rect =  padding + hero icon + padding + text + padding

    // Hero icon (fixed scale relative to height)
    int heroIconSize = spec.preferredHeight * spec.heroIconScale;

    // Remaining width for text
    int totalWidth = baseRect.right() - baseRect.left();
    int textWidth = totalWidth - (3 * padding) - heroIconSize;

    // Measure wrapped text height
    QFontMetrics fm(option.font);
    QRect textBounding = fm.boundingRect(QRect(baseRect.left() + (2 * padding) + heroIconSize, baseRect.top(), textWidth, INT_MAX),
                                         Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap,
                                         character->combatLog());

    int textHeight = textBounding.height();

    // Total height should accommodate icon and text plus padding
    int totalHeight = qMax(heroIconSize, textHeight) + padding * 2;

    return QSize(option.rect.width(), totalHeight);
}

