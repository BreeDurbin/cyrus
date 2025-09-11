
#pragma once
#include <QRect>

namespace LayoutUtils {

    inline QRect padRectangle(const QRect& base, int padding) {
        return base.adjusted(padding, padding, -padding, -padding);
    }

    inline QRect buildHeroIconRect(const QRect& base, double heroIconScale = 0.8) {
        int size = static_cast<int>(base.height() * heroIconScale);
        return QRect(base.left(), base.top(), size, size);
    }

    inline QRect buildInitiativeRect(const QRect& left, int padding, int width) {
        return QRect(left.right() + padding, left.top(),
                    width, left.height());
    }

    inline QRect buildTextRect(const QRect& left,
                               const QRect& right,
                               int padding) {
        return QRect(left.right() + padding,
                     left.top(),
                     right.left() - (left.right() + 2 * padding),
                     left.height());
    }

    inline QRect buildDeleteRect(const QRect& base, int padding, double scale = 0.6) {
        const int rectSize = static_cast<int>(base.height() * scale);
        return QRect(base.right() - rectSize - padding,
                     base.top() + (base.height() - rectSize) / 2,
                     rectSize,
                     rectSize);
    }

} // namespace LayoutUtils





