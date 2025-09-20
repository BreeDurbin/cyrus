#pragma once

#include "LayoutEngine.h"
#include "Stepper.h"

struct RosterLayout : Layout {
    QRect baseRect;
    QRect heroIconRect;
    QRect factionFrame;
    StepperRects factionStepperRects;
    QRect textRect;
    QRect cloneButtonRect;
    QRect deleteButtonRect;
};

struct RosterLayoutEngine : LayoutEngine {
        Q_OBJECT
    public: 

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
        void deleteButtonClicked(const QModelIndex& index);
        void cloneRosterMemberClicked(const QModelIndex& index);
        void nextFactionClicked(const QModelIndex& index);
        void previousFactionClicked(const QModelIndex& index);
        
};
