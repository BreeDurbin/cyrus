
#pragma once

#include <QRect>

namespace LayoutUtils {

    inline QRect padRectangle(const QRect& base, int padding) {
        return base.adjusted(padding, padding, -padding, -padding);
    }


    struct LeftAnchorBuilder {
        QRect baseRect;
        int cursorX;

        LeftAnchorBuilder(const QRect& base)
            : baseRect(base), cursorX(base.left()) {}

        // Build a left-anchored icon
        QRect icon(double scale, int padding) {
            int size = static_cast<int>(baseRect.height() * scale);
            QRect r(cursorX, baseRect.top() + (baseRect.height() - size)/2, size, size);
            cursorX += size + padding;
            return r;
        }

        QRect text(int width, int padding, int height = -1) {
            if (height < 0) height = baseRect.height();
            int y = baseRect.top() + (baseRect.height() - height) / 2;
            QRect r(cursorX, y, width, height);
            cursorX += width + padding;
            return r;
        }

        // Build a generic control (like a stepper)
        QRect control(int width, int height, int padding) {
            QRect r(cursorX, baseRect.top() + (baseRect.height() - height)/2, width, height);
            cursorX += width + padding;
            return r;
        }
    };

    struct RightAnchorBuilder {
        QRect baseRect;
        int cursorX;

        RightAnchorBuilder(const QRect& base)
            : baseRect(base), cursorX(base.right()) {}

        QRect icon(double scale, int padding) {
            int size = static_cast<int>(baseRect.height() * scale);
            cursorX -= size; // move left for this rect
            QRect r(cursorX, baseRect.top() + (baseRect.height() - size)/2, size, size);
            cursorX -= padding; // prepare for next
            return r;
        }

        QRect text(int width, int padding, int height = -1) {
            if (height < 0) height = baseRect.height();
            int y = baseRect.top() + (baseRect.height() - height) / 2;
            QRect r(cursorX, y, width, height);
            cursorX += width + padding;
            return r;
        }

        QRect control(int width, int height, int padding) {
            cursorX -= width;
            QRect r(cursorX, baseRect.top() + (baseRect.height() - height)/2, width, height);
            cursorX -= padding;
            return r;
        }
    };


    inline QRect buildMainRowRect(const QRect& base, bool isExpanded, int padding) {
        QRect rowRect;

        if (isExpanded) {
            // Take the top half of the base rect
            rowRect = QRect(
                base.left(),
                base.top(),
                base.width(),
                base.height() / 2
            );
        } else {
            // Use the full height when collapsed
            rowRect = base;
        }

        // Apply horizontal and vertical padding
        rowRect.adjust( 0, padding, -(2 * padding), -padding);

        return rowRect;
    }

    inline QRect buildDropdownRect(const QRect& base, const QRect& mainRow, int padding) {
        // Remaining space below mainRow
        if (mainRow.bottom() >= base.bottom()) return QRect(); // no space for dropdown

        QRect dropdown(
            base.left(),
            mainRow.bottom(),
            base.width(),
            base.bottom() - mainRow.bottom()
        );

        // Apply horizontal and vertical padding
        dropdown.adjust(2 * padding, padding, -(2 * padding), -padding);

        return dropdown;
    }


} // namespace LayoutUtils





