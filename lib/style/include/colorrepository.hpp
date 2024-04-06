#pragma once

#include <QColor>
#include <QPalette>


namespace ColorRepository
{
    QPalette standardPalette();
    void setDarkMode(bool dark);

    QColor windowBackground();
    QColor baseBackground();
    QColor text();

    QColor pressedTextColor();
    QColor hoverTextColor();

    QColor pressedOutlineColor();
    QBrush hoverOutlineBrush(const QRect &rect);
    QColor buttonOutlineColor();

    QColor buttonPressedBackground();
    QColor buttonHoveredBackground();
    QColor buttonBackground();

}
