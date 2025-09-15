#include "PainterUtils.h"
#include "ColorRepository.h"
#include "StyleRepository.h"

namespace PainterUtils {

    void paintIcon(QPainter* painter, const QIcon& icon, const QRect& rect,
                bool selected, bool hovered, bool pressed, int iconSize) {
        if (pressed) {
            // Pressed-down look: darker background + inner shadow effect
            QColor pressColor = ColorRepository::buttonHoveredBackground().darker(150);
            QRadialGradient pressGlow(rect.center(), iconSize * 1.4);
            pressGlow.setColorAt(0.0, QColor(pressColor.red(), pressColor.green(), pressColor.blue(), 200));
            pressGlow.setColorAt(0.6, QColor(pressColor.red(), pressColor.green(), pressColor.blue(), 100));
            pressGlow.setColorAt(1.0, QColor(pressColor.red(), pressColor.green(), pressColor.blue(), 0));

            painter->setBrush(pressGlow);
            painter->setPen(QPen(ColorRepository::selectedForeground(), 1.0));
            painter->drawEllipse(rect.adjusted(-3, -3, 3, 3));
        }
        else if (selected || hovered) {
            QColor glowColor = ColorRepository::buttonHoveredBackground();
            QRadialGradient glow(rect.center(), iconSize * 1.6);
            glow.setColorAt(0.0, QColor(glowColor.red(), glowColor.green(), glowColor.blue(), 230));
            glow.setColorAt(0.4, QColor(glowColor.red(), glowColor.green(), glowColor.blue(), 120));
            glow.setColorAt(1.0, QColor(glowColor.red(), glowColor.green(), glowColor.blue(), 0));

            painter->setBrush(glow);
            painter->setPen(selected ? QPen(ColorRepository::selectedForeground(), 1.5) : Qt::NoPen);
            painter->drawEllipse(rect.adjusted(-4, -4, 4, 4));
        }

        // Shift the icon slightly when pressed (makes it feel clickable)
        QRect iconRect = rect;
        if (pressed) {
            iconRect.translate(1, 1);
        }

        icon.paint(painter, iconRect, Qt::AlignCenter,
                pressed ? QIcon::Active : QIcon::Normal, QIcon::On);
    }



} // namespace PainterUtils
