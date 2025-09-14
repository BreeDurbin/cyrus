#include "PainterUtils.h"
#include "ColorRepository.h"
#include "StyleRepository.h"

namespace PainterUtils {

void paintStepper(QPainter* painter, const StepperRects& s, int value) {
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

void paintDeleteButton(QPainter* painter, const QRect& rect, bool hovered) {
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

void paintActionIcon(QPainter* painter, const QIcon& icon, const QRect& rect,
                                         bool selected, bool hovered, int iconSize) {
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


} // namespace PainterUtils
