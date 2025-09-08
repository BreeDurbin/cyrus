#pragma once

#include <QColor>
#include <QPalette>


namespace ColorRepository
{ 
    QPalette standardPalette();

    QColor windowBackground();
    QColor baseBackground();
    QColor text();

    QColor selectedForeground();
    QColor selectedBackground();
    QColor segmentBackground();

    QColor pressedTextColor();
    QColor hoverTextColor();

    QColor pressedOutlineColor();
    QBrush hoverOutlineBrush(const QRect &rect);
    QColor buttonOutlineColor();

    QColor buttonPressedBackground();
    QColor buttonHoveredBackground();
    QColor buttonBackground();

    QColor progressArcColor();

}