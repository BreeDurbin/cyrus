#include "PainterUtils.h"
#include "ColorRepository.h"
#include "StyleRepository.h"
#include <QIcon>

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


    void paintStyledLineEdit(QPainter* painter, const QRect& rect,
                         const QString& text, const QString& placeholder,
                         bool hasFocus, bool isEnabled)
    {
        painter->save();

        // --- Background ---
        QColor bgColor = isEnabled ? ColorRepository::baseBackground() 
                                : ColorRepository::buttonBackground();
        painter->setBrush(bgColor);
        QColor borderColor = hasFocus ? ColorRepository::pressedOutlineColor()
                                    : ColorRepository::buttonOutlineColor();
        painter->setPen(borderColor);
        painter->drawRoundedRect(rect, 6, 6);

        // --- Text ---
        painter->setPen(isEnabled ? ColorRepository::text()
                                : ColorRepository::hoverTextColor());
        QFont font(StyleRepository::textFontFamily());
        painter->setFont(font);

        QFontMetrics fm(font);
        int paddingLeft = 8;  // match padding in stylesheet
        int paddingTop = 4;

        QRect textRect = rect.adjusted(paddingLeft, paddingTop, -paddingLeft, -paddingTop);

        if (!text.isEmpty()) {
            painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, text);
        } else {
            // Placeholder text in italic
            QFont placeholderFont = font;
            placeholderFont.setItalic(true);
            painter->setFont(placeholderFont);
            painter->setPen(ColorRepository::placeholderText());
            painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, placeholder);
        }

        painter->restore();
    }



} // namespace PainterUtils
