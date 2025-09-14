#pragma once

#include "LayoutEngine.h"
#include "Stepper.h"

struct RosterLayout : Layout {
    QRect baseRect;
    QRect heroIconRect;
    QRect initiativeFrame;
    StepperRects initiativeStepperRects;
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
            bool isActiveIndex = false,
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
            const CastState& castState,
            bool isActiveIndex,
            bool isExpanded,
            const QPoint& localCursor ) const override;

        int minimumWidth(
            const std::shared_ptr<Character>& character) const override;

    signals:
        void deleteButtonClicked(const QModelIndex& index);
        void cloneRosterMemberClicked(const QModelIndex& index);
        void incrementRosterMemberInitiativeClicked(const QModelIndex& index);
        void decrementRosterMemberInitiativeClicked(const QModelIndex& index);
        
};
