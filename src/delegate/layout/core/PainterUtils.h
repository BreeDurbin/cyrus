#pragma once

#include <QPainter>
#include "Character.h"
#include <unordered_map>
#include <QUuid>
#include "CastState.h"
#include "Stepper.h"
#include "CharacterLayoutEngine.h"


namespace PainterUtils {
    void paintIcon(QPainter* painter, const QIcon& icon, const QRect& rect,
                bool selected, bool hovered, bool pressed, int iconSize);
} // namespace PainterUtils
