
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


} // namespace LayoutUtils





