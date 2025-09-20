#pragma once

#include <QColor>
#include <QPalette>
#include "Enums.h"


namespace ColorRepository
{ 
    QPalette standardPalette();

    QColor windowBackground();
    QColor baseBackground();
    QColor highlight();
    QColor text();
    QColor placeholderText();

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

    QColor colorForFaction(Cyrus::Faction f);
}