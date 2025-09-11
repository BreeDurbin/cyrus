#pragma once
#include <QPainter>
#include "Character.h"
#include "InitiativeLayout.h"
#include <unordered_map>
#include <QUuid>
#include "CastState.h"  // assuming this is where CastState is defined

namespace PainterUtils {
    void paintStepper(QPainter* painter,
                      const InitiativeLayout::StepperRects& rects,
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

    void paintAttackDropdown(QPainter* painter,
                             const QRect& dropdownRect,
                             const Character::LayoutSpec& spec);

    void paintCastDropdown(QPainter* painter,
                           const Character& character,
                           const InitiativeLayout::InitiativeRects& rects,
                           const CastState& castState);
} // namespace PainterUtils
