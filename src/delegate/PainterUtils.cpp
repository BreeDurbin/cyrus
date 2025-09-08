#include "PainterUtils.h"
#include "style/ColorRepository.h"
#include "style/StyleRepository.h"

namespace PainterUtils {

void drawStepper(QPainter* painter, const StepperRects& s, int value) {
    painter->setPen(QPen(ColorRepository::buttonOutlineColor(), 1.0));
    painter->setBrush(ColorRepository::buttonBackground());
    painter->drawRoundedRect(s.minusRect, 4, 4);
    painter->setPen(ColorRepository::text());
    painter->drawText(s.minusRect, Qt::AlignCenter, "−");

    painter->setPen(QPen(ColorRepository::buttonOutlineColor(), 1.0));
    painter->setBrush(Qt::white);
    painter->drawRoundedRect(s.valueRect, 4, 4);
    painter->setPen(Qt::black);
    painter->drawText(s.valueRect, Qt::AlignCenter, QString::number(value));

    painter->setPen(QPen(ColorRepository::buttonOutlineColor(), 1.0));
    painter->setBrush(ColorRepository::buttonBackground());
    painter->drawRoundedRect(s.plusRect, 4, 4);
    painter->setPen(ColorRepository::text());
    painter->drawText(s.plusRect, Qt::AlignCenter, "+");
}

void InitiativeDelegate::paintDeleteButton(QPainter* painter, const QRect& rect, bool hovered) const {
    const QColor fill = hovered
        ? ColorRepository::buttonHoveredBackground()
        : ColorRepository::buttonBackground();

    painter->setBrush(fill);
    painter->setPen(QPen(ColorRepository::buttonOutlineColor(), 1.0));
    painter->drawEllipse(rect);

    painter->setFont(StyleRepository::labelFont(14, true));
    painter->setPen(ColorRepository::text());
    painter->drawText(rect, Qt::AlignCenter, "X");
}

void InitiativeDelegate::paintActionIcon(QPainter* painter, const QIcon& icon, const QRect& rect,
                                         bool selected, bool hovered, int iconSize) const {
    if (selected || hovered) {
        QColor glowColor = ColorRepository::buttonHoveredBackground();
        QRadialGradient glow(rect.center(), iconSize * 1.6);
        glow.setColorAt(0.0, QColor(glowColor.red(), glowColor.green(), glowColor.blue(), 230));
        glow.setColorAt(0.4, QColor(glowColor.red(), glowColor.green(), glowColor.blue(), 120));
        glow.setColorAt(1.0, QColor(glowColor.red(), glowColor.green(), glowColor.blue(), 0));

        painter->setBrush(glow);
        painter->setPen(selected ? QPen(ColorRepository::selectedForeground(), 1.5) : Qt::NoPen);
        painter->drawEllipse(rect.adjusted(-4, -4, 4, 4));
    }

    icon.paint(painter, rect, Qt::AlignCenter, QIcon::Normal, QIcon::On);
}   


void drawAttackDropdown(QPainter* painter, const QRect& dropdownRect,
                        const LayoutSpec& spec) {
    const int h = dropdownRect.height();
    const int y = dropdownRect.top() + (h - 24) / 2;

    QRect spinRect(dropdownRect.left() + spec.padding, y, 64, 24);
    QRect okRect(spinRect.right() + spec.padding, y, 56, 24);

    painter->setPen(QPen(ColorRepository::buttonOutlineColor(), 1.0));
    painter->setBrush(ColorRepository::buttonBackground());
    painter->drawRoundedRect(spinRect, 6, 6);
    painter->setPen(ColorRepository::text());
    painter->drawText(spinRect, Qt::AlignCenter, "1");

    painter->setPen(QPen(ColorRepository::buttonOutlineColor(), 1.0));
    painter->setBrush(ColorRepository::buttonBackground());
    painter->drawEllipse(okRect);
    painter->setPen(ColorRepository::text());
    painter->drawText(okRect, Qt::AlignCenter, "OK");
}

void drawCastDropdown(QPainter* painter, const Character& character,
                      const InitiativeRects& rects,
                      const std::unordered_map<QUuid, CastState>& castState) {
    painter->setFont(StyleRepository::labelFont(14, false));
    painter->setPen(ColorRepository::text());
    painter->drawText(rects.cast.castingTimeLabel, Qt::AlignVCenter | Qt::AlignLeft, "Speed");
    painter->drawText(rects.cast.durationLabel,    Qt::AlignVCenter | Qt::AlignLeft, "Dur");

    // Spell edit
    painter->setPen(QPen(ColorRepository::buttonOutlineColor(), 1.0));
    painter->setBrush(Qt::white);
    painter->drawRoundedRect(rects.cast.spellEdit, 6, 6);

    painter->setPen(Qt::black);
    auto it = castState.find(character.uuid());
    const QString spellText = (it != castState.end() && !it->second.spellName.isEmpty())
                                ? it->second.spellName
                                : QStringLiteral("Spell name…");
    painter->drawText(rects.cast.spellEdit.adjusted(6, 0, -6, 0),
                      Qt::AlignVCenter | Qt::AlignLeft, spellText);

    // Steppers
    const int castingTimeValue = it != castState.end() ? it->second.castingTime : 1;
    const int durationValue    = it != castState.end() ? it->second.duration : 0;

    drawStepper(painter, rects.cast.castingTimeStepperRects, castingTimeValue);
    drawStepper(painter, rects.cast.durationStepperRects, durationValue);

    // OK button
    painter->setPen(QPen(ColorRepository::buttonOutlineColor(), 1.0));
    painter->setBrush(ColorRepository::buttonBackground());
    painter->drawEllipse(rects.cast.okRect);
    painter->setPen(ColorRepository::text());
    painter->drawText(rects.cast.okRect, Qt::AlignCenter, "OK");
}

} // namespace PainterUtils
