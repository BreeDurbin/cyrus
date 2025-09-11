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
void CombatLogDelegate::paint(QPainter* painter,
                              const QStyleOptionViewItem& option,
                              const QModelIndex& index) const
{
    painter->save();

    const QVariant charVar = index.data(Cyrus::CharacterRole);
    if (!charVar.canConvert<std::shared_ptr<Character>>()) { return; }
    auto character = charVar.value<std::shared_ptr<Character>>();

    const Character::LayoutSpec& spec = character->layoutSpec();

    // Base rects
    QRect baseRect    = LayoutUtils::padRectangle(option.rect, spec.padding);
    QRect heroIconRect = LayoutUtils::buildHeroIconRect(baseRect, spec.heroIconScale);
    // build text rect right side is not a rectangle it is the right side of base rect
    QRect rightBoundary(baseRect.right(), baseRect.top(), 0, baseRect.height());
    QRect textRect     = LayoutUtils::buildTextRect(heroIconRect, rightBoundary, spec.padding);

    // Background
    painter->setBrush(option.state & QStyle::State_Selected
                      ? ColorRepository::selectedBackground()
                      : ColorRepository::baseBackground());
    painter->setPen(Qt::NoPen);
    painter->drawRect(baseRect);

    // Hero icon (from Character, not data roles)
    const QIcon heroIcon = IconRepository::iconFor(character->characterType());
    if (!heroIcon.isNull()) {
        heroIcon.paint(painter, heroIconRect, Qt::AlignCenter,
                    QIcon::Normal, QIcon::On);
    }


    // Text (from Character)
    painter->setPen(ColorRepository::text());
    painter->drawText(textRect,
                    Qt::AlignVCenter | Qt::AlignLeft | Qt::TextWordWrap,
                    character->combatLog());


    painter->restore();
}


QSize CombatLogDelegate::sizeHint(const QStyleOptionViewItem& option,
                                  const QModelIndex& index) const
{
    const QVariant charVar = index.data(Cyrus::CharacterRole);
    if (!charVar.canConvert<std::shared_ptr<Character>>()) {
        return QStyledItemDelegate::sizeHint(option, index);
    }
    auto character = charVar.value<std::shared_ptr<Character>>();
    const Character::LayoutSpec& spec = character->layoutSpec();

    // Base rects
    QRect baseRect    = LayoutUtils::padRectangle(option.rect, spec.padding);
    //we cant scale based on item height because we are calculating item height now
    int defaultIconHeight = 32; 
    QRect heroIconRect(baseRect.left(),
                    baseRect.top(),
                    defaultIconHeight, // width == height for square icon
                    defaultIconHeight);

    // Text rect area (everything to the right of hero icon until baseRect.right)
    QRect rightBoundary(baseRect.right(), baseRect.top(), 0, baseRect.height());
    QRect textRect     = LayoutUtils::buildTextRect(heroIconRect, rightBoundary, spec.padding);

    // Measure wrapped text height
    QFontMetrics fm(option.font);
    QRect textBounding = fm.boundingRect(QRect(0, 0, textRect.width(), INT_MAX),
                                         Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap,
                                         character->combatLog());

    int textHeight = textBounding.height();

    // Total height should fit both the hero icon and the wrapped text, plus padding
    int totalHeight = qMax(heroIconRect.height(), textHeight) + spec.padding * 2;

    return QSize(option.rect.width(), totalHeight);
}
