#include "ColorRepository.h"

#include <QApplication>
#include <QBrush>
#include <QToolTip>

// --- Base Colors (softer, fantasy inspired) ---
QColor m_backgroundPrimary()   { return QColor(0x16, 0x1A, 0x1F); } // #161A1F (soft charcoal, bluish)
QColor m_backgroundSecondary() { return QColor(0x22, 0x27, 0x2E); } // #22272E (cool slate)
QColor m_surfaceHighlight()    { return QColor(0x2C, 0x33, 0x3D); } // #2C333D (muted highlight)

// --- Blue (enchanted sapphire tones) ---
QColor m_bluePrimary() { return QColor(0x3A, 0x8D, 0xD9); } // #3A8DD9 (sapphire blue, AAA on base)
QColor m_blueHover()   { return QColor(0x63, 0xB0, 0xEB); } // #63B0EB (lighter, airy hover — AAA contrast kept)
QColor m_blueMuted()   { return QColor(0x86, 0xAF, 0xD6); } // #86AFD6 (muted steel blue — higher contrast than #9DC9F0)

// --- Gold (antique fantasy metals) ---
QColor m_goldHighlight() { return QColor(0xE9, 0xB4, 0x2F); } // #E9B42F (rich antique gold)
QColor m_goldHover()     { return QColor(0xF2, 0xC7, 0x52); } // #F2C752 (bright hover, still soft enough)
QColor m_goldMuted()     { return QColor(0x8C, 0x6E, 0x24); } // #8C6E24 (bronze-brown, better dark contrast)

// --- Text (derived from parchment #FCF1C4, tuned for contrast) ---
QColor m_textPrimary()   { return QColor(0xFC, 0xF1, 0xC4); } // #FCF1C4 — Contrast 9.0–14:1 (AAA everywhere)
QColor m_textSecondary() { return QColor(0xC2, 0xB7, 0x8C); } // #C2B78C — Contrast ~6.5–10:1 (AA/AAA, muted but readable)
QColor m_textDisabled()  { return QColor(0x8E, 0x85, 0x63); } // #8E8563 — Contrast ~4.7–7.5:1 (AA, faded but legible)



QPalette ColorRepository::standardPalette()
{
    QPalette pal;

    pal.setColor(QPalette::Window, windowBackground());
    pal.setColor(QPalette::Base, baseBackground());

    pal.setColor(QPalette::WindowText, text());
    pal.setColor(QPalette::Text, text());

    // Text color on buttons
    pal.setColor(QPalette::ButtonText, text());

    pal.setColor(QPalette::ToolTipText, text());

    QToolTip::setPalette(pal);
    return pal;
}

QColor ColorRepository::windowBackground()
{
    return m_backgroundPrimary();
}

QColor ColorRepository::baseBackground()
{
    return m_backgroundSecondary();
}

QColor ColorRepository::text()
{
    return m_textPrimary();
}

QColor ColorRepository::selectedForeground()
{
    return m_goldHighlight();
}

QColor ColorRepository::selectedBackground()
{
    return m_bluePrimary(); //return m_blueHover();
}

QColor ColorRepository::segmentBackground()
{
    return m_bluePrimary();
}

QColor ColorRepository::pressedTextColor()
{
    return m_textPrimary();
}

QColor ColorRepository::hoverTextColor()
{
    return m_textSecondary();
}

QColor ColorRepository::pressedOutlineColor()
{
    return m_goldMuted();
}

QColor ColorRepository::buttonOutlineColor()
{
    return m_goldHighlight();
}

QBrush ColorRepository::hoverOutlineBrush(const QRect &rect)
{
    const qreal w = rect.width();
    const qreal h = rect.height();
    const qreal xmid = w * 0.54;
    const qreal xoffset = (h * h) / (2 * w);
    const qreal x0 = xmid - xoffset;
    const qreal x1 = xmid + xoffset;

    QLinearGradient gradient(x0, h, x1, 0);
    gradient.setColorAt(0.0, m_goldMuted());
    gradient.setColorAt(1.0, m_goldHover());
    return QBrush(gradient);
}

QColor ColorRepository::buttonPressedBackground()
{
    return m_goldHighlight();
}

QColor ColorRepository::buttonHoveredBackground()
{
    QColor color = m_goldHover();
    color.setAlphaF(0.2);
    return color;
}

QColor ColorRepository::buttonBackground()
{
    return m_surfaceHighlight();
}

QColor ColorRepository::progressArcColor(){
    return m_goldHighlight();
}
