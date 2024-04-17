#pragma once

#include <QSize>

class QPainter;
class QPushButton;
class QStyleOptionButton;
class QWidget;

class PushButtonStyleHelper
{
public:
    void setupPainterForShape(const QStyleOptionButton *option, QPainter *painter, const QWidget *widget);
    void drawButtonShape(const QStyleOptionButton *option, QPainter *painter, const QWidget *widget);
    QSize sizeFromContents(const QStyleOptionButton *option, QSize contentsSize, const QWidget *widget) const;
    void adjustTextPalette(QStyleOptionButton *option) const;
};

