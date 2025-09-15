#pragma once

#include "LayoutEngine.h"
#include "Stepper.h"

#include "Character.h"
#include <QRect>
#include "CastState.h"
#include <QModelIndex>
#include <QUuid>
#include "Enums.h"

struct CastRects {
    QRect spellEdit;
    QRect castingTimeLabel;
    QRect castingTimeFrame;
    StepperRects castingTimeStepperRects;
    QRect durationLabel;
    QRect durationFrame;
    StepperRects durationStepperRects;
    QRect submitButtonRect;
};

struct AttackRects {
    QRect attackAmountFrame;
    StepperRects attackAmountStepperRects;
    QRect submitButtonRect;
};

struct CharacterRects {
    QRect heroIconRect;
    QRect initiativeRect;
    QRect deleteButtonRect;
    QRect iconSelectorRect;
    QRect rightAnchorRect;
    QRect textRect;
    QVector<QRect> actionIconRects;
};

struct CharacterLayout : Layout {
    QRect mainRowRect;
    QRect dropdownRect;
    CharacterRects top;
    std::optional<CastRects> cast;  // only present if actionType == Cast
    std::optional<AttackRects> attack; // only present if actionType == Attack (Implement later)
};

struct CharacterLayoutEngine : LayoutEngine {
        Q_OBJECT
    public: 

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
            const CastState castState,
            bool isActiveIndex,
            bool isExpanded,
            const QPoint& localCursor ) const override;

        int minimumWidth(
            const std::shared_ptr<Character>& character) const override;

    signals:
        void spellNameEdited(const QUuid& id, const QString& name) const;
        void decrementCastingTimeClicked(const QUuid& id);
        void incrementCastingTimeClicked(const QUuid& id);
        void decrementDurationClicked(const QUuid& id);
        void incrementDurationClicked(const QUuid& id);
        void castSubmitClicked(const QUuid& id, const std::shared_ptr<Character>& character);
        void deleteItemClicked(const QModelIndex& index) const; // delete button clicked
        void iconSelectorClicked(const QModelIndex& index, Cyrus::ActionType actionType); // icon selector hit
        void decrementAttackAmountClicked(const QModelIndex& index);
        void incrementAttackAmountClicked(const QModelIndex& index);
        void attackSubmitClicked(const QModelIndex& index);

    private:

        // Helpers, all static
        static QRect buildIconSelectorRect(const QRect& deleteRect, int rowHeight,
                                        const Character::LayoutSpec& spec);
        static QVector<QRect> buildActionIconRects(const QRect& iconSelectorRect,
                                                int rowHeight,
                                                const Character::LayoutSpec& spec);
        static QRect buildMainRowRect(const QRect& base, bool isExpanded, int padding);
        static QRect buildDropdownRect(const QRect& base, const QRect& mainRow, int padding);
        static StepperRects buildStepperRects(const QRect& frame,
                                            int buttonW, int valueW,
                                            int h, int padding);
        static AttackRects buildAttackRects(const QRect& dropdownRect,
                                                    const Character::LayoutSpec& spec);
        static CastRects buildCastRects(const QRect& dropdownRect,
                                        const Character::LayoutSpec& spec);

        //--------- Helpers -----------
        static int minimumCastRowWidth(const Character& character);
        static int castingTimeLabelWidth();
        static int durationLabelWidth();
        bool checkIfCharacter(const std::shared_ptr<Character>& character) const;

        void paintAttackDropdown(QPainter* painter,
                            const QRect& dropdownRect,
                            const AttackRects& attack,
                            const std::shared_ptr<Character>& character,
                            const QPoint& localCursor) const;

        void paintCastDropdown(QPainter* painter,
                            const QRect& dropdownRect,
                            const CastRects& cast,
                            const CastState& castState,
                            const QPoint& localCursor) const;

};   

    

