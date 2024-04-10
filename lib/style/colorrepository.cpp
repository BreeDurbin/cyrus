#include "include/colorrepository.hpp"

#include <QApplication>
#include <QBrush>
#include <QToolTip>

static bool s_darkMode = true;

QColor m_parchment(){ return QColor(254, 242, 200); }
QColor m_lightGold(){ return QColor(197,157,98); }
QColor m_gold(){ return QColor(255,215,0); }
QColor m_darkGold(){ return QColor(184,134,11); }
QColor m_lightBlue(){ return QColor(0,0,255); }
QColor m_blue(){ return QColor(0,0,128); }
QColor m_darkBlue(){ return QColor(11,35,59); }

QPalette ColorRepository::standardPalette()
{
    QPalette pal;
    // TODO brush with noise.png
    pal.setColor(QPalette::Window, windowBackground());
    pal.setColor(QPalette::Base, baseBackground());
    pal.setColor(QPalette::WindowText, text());
    pal.setColor(QPalette::Text, text());

    // Text color on buttons
    pal.setColor(QPalette::ButtonText, text());

    // pal.setColor(QPalette::ToolTipBase, baseBackground());
    pal.setColor(QPalette::ToolTipText, text());

    QToolTip::setPalette(pal);

    return pal;
}

void ColorRepository::setDarkMode(bool dark)
{
    s_darkMode = dark;
    qApp->setPalette(standardPalette());
}

QColor ColorRepository::windowBackground()
{
    return m_darkBlue();
}

QColor ColorRepository::baseBackground()
{
    return m_darkBlue();
}

QColor ColorRepository::text()
{
    return m_parchment();
}

QColor ColorRepository::pressedTextColor()
{
    return m_parchment();
}

QColor ColorRepository::hoverTextColor()
{
    return m_parchment();
}

QColor ColorRepository::pressedOutlineColor()
{
    return m_darkGold();
}

QColor ColorRepository::buttonOutlineColor()
{
    return m_lightGold();
}

QBrush ColorRepository::hoverOutlineBrush(const QRect &rect)
{
    // Instructions from the designer:
    // "Draw line passing by center of rectangle (+4% to the right)
    // and that is perpendicular to the topleft-bottomright diagonal.
    // This line intersects the top and bottom in two points, which are the gradient stops"

    const qreal w = rect.width();
    const qreal h = rect.height();
    const qreal xmid = w * 0.54;
    const qreal xoffset = (h * h) / (2 * w); // Proportionality: xoffset / (h/2) = h / w
    const qreal x0 = xmid - xoffset;
    const qreal x1 = xmid + xoffset;

    QLinearGradient gradient(x0, h, x1, 0);
    gradient.setColorAt(0.0, QColor(0x53, 0x94, 0x9f));
    gradient.setColorAt(1.0, QColor(0x75, 0x55, 0x79));
    return QBrush(gradient);
}

QColor ColorRepository::buttonPressedBackground()
{
    return m_lightGold();
}

QColor ColorRepository::buttonHoveredBackground()
{
    QColor color = buttonPressedBackground();
    color.setAlphaF(0.2);
    return color;
}

QColor ColorRepository::buttonBackground()
{
    return m_darkBlue();
}