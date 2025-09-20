#pragma once

#include <QPainter>
#include <unordered_map>
#include <QUuid>



namespace PainterUtils {
    void paintIcon(QPainter* painter, const QIcon& icon, const QRect& rect,
                bool selected, bool hovered, bool pressed, int iconSize);
} // namespace PainterUtils
