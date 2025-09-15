#pragma once

#include <QPainter>
#include <QRect>
#include "ColorRepository.h"

struct StepperRects {
    QRect minusRect;
    QRect valueRect;
    QRect plusRect;
};

namespace Stepper {

    inline StepperRects buildStepperRects(const QRect& frame,
                                          int buttonW,
                                          int valueW,
                                          int h,
                                          int padding) 
    {
        StepperRects s;

        // shorter button height (e.g. 80% of value height)
        int buttonH = static_cast<int>(h * 0.8);
        int y = frame.top() + (frame.height() - h) / 2;

        int x = frame.left();

        // minus button (centered vertically against full stepper height)
        s.minusRect = QRect(x,
                            y + (h - buttonH) / 2,
                            buttonW,
                            buttonH);
        x += buttonW + padding;

        // value box (full height)
        s.valueRect = QRect(x, y, valueW, h);
        x += valueW + padding;

        // plus button (same as minus)
        s.plusRect = QRect(x,
                           y + (h - buttonH) / 2,
                           buttonW,
                           buttonH);

        return s;
    }

    template <typename T>
    void paintStepper(QPainter* painter, const StepperRects& s, const T& value) {
        // Draw minus button
        painter->setPen(QPen(ColorRepository::buttonOutlineColor(), 1.0));
        painter->setBrush(ColorRepository::buttonBackground());
        painter->drawRoundedRect(s.minusRect, 4, 4);
        painter->setPen(ColorRepository::text());
        painter->drawText(s.minusRect, Qt::AlignCenter, "−");

        // Convert value to QString
        QString valueStr;
        if constexpr (std::is_arithmetic_v<T>) {
            valueStr = QString::number(value);        // numeric types
        } else if constexpr (requires { value.toString(); }) {
            valueStr = value.toString();             // objects with toString()
        } else {
            valueStr = QString::fromStdString(std::to_string(value)); // fallback
        }

        // Draw value
        painter->setPen(QPen(ColorRepository::buttonOutlineColor(), 1.0));
        painter->setBrush(Qt::white);
        painter->drawRoundedRect(s.valueRect, 4, 4);
        painter->setPen(Qt::black);
        painter->drawText(s.valueRect, Qt::AlignCenter, valueStr);

        // Draw plus button
        painter->setPen(QPen(ColorRepository::buttonOutlineColor(), 1.0));
        painter->setBrush(ColorRepository::buttonBackground());
        painter->drawRoundedRect(s.plusRect, 4, 4);
        painter->setPen(ColorRepository::text());
        painter->drawText(s.plusRect, Qt::AlignCenter, "+");
    }

} // namespace Stepper
