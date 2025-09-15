#pragma once

#include "LayoutEngine.h"

#include "Character.h"
#include <QRect>
#include "CastState.h"
#include <memory>

struct CastActionLayout : Layout {
    QRect baseRect;
    QRect heroIconRect;
    QRect initiativeRect;
    QRect durationIconRect;
    QRect durationTextRect;
    QRect textRect;
    QRect deleteButtonRect;
};

struct CastActionLayoutEngine : LayoutEngine {
        Q_OBJECT

    std::shared_ptr<Layout> calculate(
        const QRect& rect,    
        const std::shared_ptr<Character>& character,
        bool isActiveIndex,
        bool isExpanded = false) const override;

    std::optional<HitCommand> hitTest(
        const QModelIndex& index,
        const std::shared_ptr<Layout>& layout,
        const std::shared_ptr<Character>& character,
        const QPoint& cursorPos) override;

    void paintLayout(
        QPainter* painter,
        const std::shared_ptr<Layout>& layout,
        const std::shared_ptr<Character>& character,
        const CastState castState,
        bool isActiveIndex,
        bool isExpanded,
        const QPoint& localCursor ) const override;

    int minimumWidth(
        const std::shared_ptr<Character>& character) const override;
                                
};