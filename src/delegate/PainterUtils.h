#pragma once
#include <QPainter>
#include "LayoutSpec.h"
#include "InitiativeRects.h"
#include "Character.h"

namespace PainterUtils {
    void drawStepper(QPainter* painter, const StepperRects& rects, int value);
    void drawDeleteButton(QPainter* painter, const QRect& rect, bool hovered) const {
    void drawActionIcon(QPainter* painter, const QIcon& icon, const QRect& rect,
                                        bool selected, bool hovered, int iconSize) const;
    void drawAttackDropdown(QPainter* painter, const QRect& dropdownRect,
                            const LayoutSpec& spec);
    void drawCastDropdown(QPainter* painter, const Character& character,
                        const InitiativeRects& rects,
                        const std::unordered_map<QUuid, CastState>& castState);

} // namespace PainterUtils
