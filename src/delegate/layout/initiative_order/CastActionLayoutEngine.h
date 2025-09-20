#pragma once

#include "LayoutEngine.h"

#include "Character.h"
#include <QRect>
#include "CastState.h"
#include <memory>

struct CastActionLayout : Layout {
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
        bool isSelectedIndex,
        bool isExpanded,
        const Cyrus::CombatSequenceState state = Cyrus::CombatSequenceState::NPC_DETERMINATION
    ) const override;

    std::optional<HitCommand> hitTest(
        const QModelIndex& index,
        const std::shared_ptr<Layout>& layout,
        const std::shared_ptr<Character>& character,
        const QPoint& cursorPos,
        const Cyrus::CombatSequenceState state = Cyrus::CombatSequenceState::NPC_DETERMINATION
    ) override;

    void paintLayout(
        QPainter* painter,
        const std::shared_ptr<Layout>& layout,
        const std::shared_ptr<Character>& character,
        const CastState castState,
        bool isSelectedIndex,
        bool isExpanded,
        const QPoint& localCursor,
        const Cyrus::CombatSequenceState state = Cyrus::CombatSequenceState::NPC_DETERMINATION
     ) const override;

    int minimumWidth(
        const std::shared_ptr<Character>& character) const override;

    
    signals:
        void deleteItemClicked(const QModelIndex& index) const;
                                
};