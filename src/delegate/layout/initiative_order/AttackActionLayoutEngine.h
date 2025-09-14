#pragma once

#include "LayoutEngine.h"

#include "Character.h"
#include <QRect>
#include "CastState.h"


struct AttackActionLayout : Layout {
    QRect baseRect;
    QRect heroIconRect;
    QRect initiativeRect;
    QRect textRect;
    QRect deleteButtonRect;
};

struct AttackActionLayoutEngine : LayoutEngine {
            Q_OBJECT
    std::shared_ptr<Layout> calculate(
        const QRect& rect,
        const std::shared_ptr<Character>& character,
        bool isActiveIndex,
        bool isExpanded) const override;

    std::optional<HitCommand> hitTest(
        const QModelIndex& index,
        const std::shared_ptr<Layout>& layout,
        const std::shared_ptr<Character>& character,
        const QPoint& cursorPos) override;

    void paintLayout(
        QPainter* painter,
        const std::shared_ptr<Layout>& layout,
        const std::shared_ptr<Character>& character,
        const CastState& castState,
        bool isActiveIndex,
        bool isExpanded,
        const QPoint& localCursor ) const override;

    int minimumWidth(
        const std::shared_ptr<Character>& character) const override;
                                
};