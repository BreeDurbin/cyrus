#pragma once

#include <QPainter>
#include "Character.h"
#include <unordered_map>
#include <QUuid>
#include "CastState.h"
#include "Stepper.h"
#include "CharacterLayoutEngine.h"


namespace PainterUtils {
    void paintStepper(QPainter* painter,
                      const StepperRects& rects,
                      int value);

    void paintDeleteButton(QPainter* painter,
                           const QRect& rect,
                           bool hovered);

    void paintActionIcon(QPainter* painter,
                         const QIcon& icon,
                         const QRect& rect,
                         bool selected,
                         bool hovered,
                         int iconSize);
} // namespace PainterUtils
