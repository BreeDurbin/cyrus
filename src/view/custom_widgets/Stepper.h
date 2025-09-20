#pragma once

#include <QPainter>
#include <QRect>
#include <QIcon>
#include <optional>
#include "ColorRepository.h"
#include "IconRepository.h"
#include "StyleRepository.h"
#include <QApplication>

namespace PainterUtils {
    void paintIcon(QPainter*, const QIcon&, const QRect&, bool, bool, bool, int);
}

// ------------------- Rects -------------------

struct StepperRects {
    QRect rightRect;   // increment
    QRect valueRect;   // center pill
    QRect leftRect;    // decrement
};

// ------------------- Hover State -------------------

struct StepperState {
    bool rightHovered = false;
    bool valueHovered = false;
    bool leftHovered  = false;
    bool rightPressed = false;
    bool leftPressed  = false; 
};

// ------------------- Builder -------------------

namespace Stepper {

    // ------------------- Hover State -------------------

    inline StepperState buildHoverState(const StepperRects& rects,
                                             const QPoint& cursorPos) {
        bool isPressed = QApplication::mouseButtons() & Qt::LeftButton;
        StepperState state;
        state.leftHovered  = rects.leftRect.contains(cursorPos);                 // left arrow
        state.valueHovered = rects.valueRect.contains(cursorPos);                // center pill
        state.rightHovered = rects.rightRect.contains(cursorPos);                // right arrow
        state.leftPressed  = rects.leftRect.contains(cursorPos) && isPressed;    // left rect pressed
        state.rightPressed = rects.rightRect.contains(cursorPos) && isPressed;   // right rect pressed
        return state;
    }

    // ------------------- Builder -------------------
    inline StepperRects buildStepperRects(const QRect& frame,
                                        int rowHeight,
                                        double scale) 
    {
        StepperRects s;

        // --- Value box dimensions (square = rowHeight x rowHeight) ---
        int valueSize = rowHeight * scale;

        // --- Arrow button dimensions (70% of value box height) ---
        int arrowSize = static_cast<int>(valueSize * 0.7);

        // --- Base width of controls without padding ---
        int baseW = arrowSize + valueSize + arrowSize;

        // --- Total width including padding ---
        double totalW = baseW / 0.95;       // baseW = 90% of totalW
        int padding = static_cast<int>(totalW * 0.025); // 2.5% on each side

        // --- Horizontal starting position to center all controls ---
        int x = frame.left() + (frame.width() - static_cast<int>(totalW)) / 2;

        // --- Left arrow (vertically centered) ---
        int arrowY = frame.top() + (frame.height() - arrowSize) / 2;
        s.leftRect = QRect(x, arrowY, arrowSize, arrowSize);
        x += arrowSize + padding;

        // --- Value box (square) ---
        int valueY = frame.top() + (frame.height() - valueSize) / 2;
        s.valueRect = QRect(x, valueY, valueSize, valueSize);
        x += valueSize + padding;

        // --- Right arrow (vertically centered) ---
        arrowY = frame.top() + (frame.height() - arrowSize) / 2;
        s.rightRect = QRect(x, arrowY, arrowSize, arrowSize);

        return s;
    }

    inline StepperRects buildStepperRects(const QRect& frame,
                                      int rowHeight,
                                      int labelWidth,
                                      double scale = 0.7)
    {
        StepperRects s;

        // --- Arrow button dimensions (square, scaled from rowHeight) ---
        int arrowSize = static_cast<int>(rowHeight * scale);

        // --- Total width ---
        int baseW = arrowSize + labelWidth + arrowSize;
        double totalW = baseW / 0.95; // baseW = 90% of totalW
        int padding = static_cast<int>(totalW * 0.025); // 2.5% of total on each side

        // --- Starting x to center the control ---
        int x = frame.left() + (frame.width() - static_cast<int>(totalW)) / 2;

        // --- Vertical centering ---
        int arrowY = frame.top() + (frame.height() - arrowSize) / 2;
        int labelY = frame.top() + (frame.height() - rowHeight) / 2;

        // --- Left arrow ---
        s.leftRect = QRect(x, arrowY, arrowSize, arrowSize);
        x += arrowSize + padding;

        // --- Label box ---
        s.valueRect = QRect(x, labelY, labelWidth, rowHeight);
        x += labelWidth + padding;

        // --- Right arrow ---
        s.rightRect = QRect(x, arrowY, arrowSize, arrowSize);

        return s;
    }




    // ------------------- Painter -------------------

    template <typename T>
    void paintStepper(QPainter* painter,
                    const StepperRects& r,
                    const StepperState& hover,
                    const T& value,
                    bool isDarkArrows = true
                ) 
    {
        painter->setRenderHint(QPainter::Antialiasing, true);
        using ::PainterUtils::paintIcon;

        // --- Left arrow ---
        paintIcon(painter,
                IconRepository::left_arrow_icon(isDarkArrows),
                r.leftRect,
                false,                // selected
                hover.leftHovered,    // hovered
                hover.leftPressed,    // pressed
                r.leftRect.height()); // iconSize

        // --- Value box ---
        painter->setPen(hover.valueHovered
                    ? QPen(ColorRepository::hoverOutlineBrush(r.valueRect), 2.0)
                    : QPen(ColorRepository::selectedForeground(), 1.5));
        painter->setBrush(ColorRepository::buttonBackground());
        painter->drawRoundedRect(r.valueRect, r.valueRect.height() / 2, r.valueRect.height() / 2);

        // --- Value content ---
        if constexpr (std::is_same_v<T, QColor>) {
            // Fill the value rect with the color
            painter->setBrush(value);
            painter->setPen(Qt::NoPen);
            // Optional: add small padding so the color fits nicely inside the rounded rect
            QRect colorRect = r.valueRect.adjusted(4, 4, -4, -4);
            painter->drawRoundedRect(colorRect, colorRect.height() / 2, colorRect.height() / 2);
        } else {
            QString valueStr;
            if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
                valueStr = QString::number(value);
            } else if constexpr (std::is_same_v<T, QString>) {
                valueStr = value;  // already a QString
            } else if constexpr (requires { value.toString(); }) {
                valueStr = value.toString();
            } else {
                valueStr = QString::fromStdString(std::to_string(value));
            }

            painter->setPen(ColorRepository::text());
            painter->drawText(r.valueRect, Qt::AlignCenter, valueStr);
        }


        // --- Right arrow ---
        paintIcon(painter,
                IconRepository::right_arrow_icon(isDarkArrows),
                r.rightRect,
                false,                 // selected
                hover.rightHovered,    // hovered
                hover.rightPressed,    // pressed
                r.rightRect.height()); // iconSize
    }

} // namespace Stepper
