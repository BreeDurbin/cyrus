#pragma once

#include <QToolButton>
#include <QPainter>
#include <QEvent>
#include "ColorRepository.h"
#include "IconRepository.h"

class CyrusToolButton : public QToolButton {
    Q_OBJECT
public:
    explicit CyrusToolButton(QWidget* parent = nullptr)
        : QToolButton(parent) {
        setMouseTracking(true); // needed for hover events
        setAutoRaise(true);     // remove default button borders
    }

protected:
    void enterEvent(QEnterEvent* event) override {
        hovered_ = true;
        update();
        QToolButton::enterEvent(event);
    }

    void leaveEvent(QEvent* event) override {
        hovered_ = false;
        update();
        QToolButton::leaveEvent(event);
    }

    void mousePressEvent(QMouseEvent* event) override {
        pressed_ = true;
        update();
        QToolButton::mousePressEvent(event);
    }

    void mouseReleaseEvent(QMouseEvent* event) override {
        pressed_ = false;
        update();
        QToolButton::mouseReleaseEvent(event);
    }

    void paintEvent(QPaintEvent* event) override {
        Q_UNUSED(event);
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        QRect rect = this->rect();

        // Draw hover or press glow
        if (pressed_) {
            QColor pressColor = ColorRepository::buttonHoveredBackground().darker(150);
            QRadialGradient pressGlow(rect.center(), rect.width() * 0.8);
            pressGlow.setColorAt(0.0, QColor(pressColor.red(), pressColor.green(), pressColor.blue(), 200));
            pressGlow.setColorAt(0.6, QColor(pressColor.red(), pressColor.green(), pressColor.blue(), 100));
            pressGlow.setColorAt(1.0, QColor(pressColor.red(), pressColor.green(), pressColor.blue(), 0));

            painter.setBrush(pressGlow);
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(rect.adjusted(-3, -3, 3, 3));
        } else if (hovered_) {
            QColor glowColor = ColorRepository::buttonHoveredBackground();
            QRadialGradient glow(rect.center(), rect.width() * 0.9);
            glow.setColorAt(0.0, QColor(glowColor.red(), glowColor.green(), glowColor.blue(), 230));
            glow.setColorAt(0.4, QColor(glowColor.red(), glowColor.green(), glowColor.blue(), 120));
            glow.setColorAt(1.0, QColor(glowColor.red(), glowColor.green(), glowColor.blue(), 0));

            painter.setBrush(glow);
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(rect.adjusted(-4, -4, 4, 4));
        }

        // Draw icon, shifted slightly if pressed
        QRect iconRect = rect;
        if (pressed_) {
            iconRect.translate(1, 1);
        }
        icon().paint(&painter, iconRect, Qt::AlignCenter,
                     pressed_ ? QIcon::Active : QIcon::Normal, QIcon::On);
    }

private:
    bool hovered_ = false;
    bool pressed_ = false;
};
