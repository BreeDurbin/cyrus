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
    QRect attackAmountLabel;
    QRect attackAmountFrame;
    StepperRects attackAmountStepperRects;
    QRect weaponSpeedLabel;
    QRect weaponSpeedFrame;
    StepperRects weaponSpeedStepperRects;
    QRect submitButtonRect;
};

struct MiscRects {
    QRect iconSelectorRect;
    QVector<QRect> miscActionIconRects;
    QRect actionCostLabel;
    QRect actionCost;
    QRect submitButtonRect;
};

struct CharacterRects {
    QRect heroIconRect;
    QRect initiativeIconRect;
    QRect initiativeFrame;
    StepperRects iniativeStepperRects;
    QRect factionRect;
    QRect deleteButtonRect;
    QRect iconSelectorRect;
    QRect rightAnchorRect;
    QRect textRect;
    QVector<QRect> actionIconRects;
};

struct CharacterLayout : Layout {
    QRect dropdownRect;
    CharacterRects top;
    std::optional<CastRects> cast;  // only present if actionType == Cast
    std::optional<AttackRects> attack; // only present if actionType == Attack
    std::optional<MiscRects> misc; // only present if actionType == Attack
};

struct CharacterLayoutEngine : LayoutEngine {
        Q_OBJECT
    public: 

        std::shared_ptr<Layout> calculate(
            const QRect& rect,
            const std::shared_ptr<Character>& character,
            bool isSelectedIndex,
            bool isExpanded, 
            const Cyrus::CombatSequenceState state
        ) const override;

        std::optional<HitCommand> hitTest(
            const QModelIndex& index,
            const std::shared_ptr<Layout>& layout,
            const std::shared_ptr<Character>& character,
            const QPoint& cursorPos,
            const Cyrus::CombatSequenceState state
        ) override;

        void paintLayout(
            QPainter* painter,
            const std::shared_ptr<Layout>& layout,
            const std::shared_ptr<Character>& character,
            const CastState castState,
            bool isSelectedIndex,
            bool isExpanded,
            const QPoint& localCursor,
            const Cyrus::CombatSequenceState state
         ) const override;

        int minimumWidth(
            const std::shared_ptr<Character>& character) const override;

    signals:
        // Main row
        void cancelActionClicked(const QModelIndex& index);
        void deleteItemClicked(const QModelIndex& index) const;
        void iconSelectorClicked(const QModelIndex& index, Cyrus::ActionType actionType);
        void decrementInitiativeClicked(const QModelIndex& index);
        void incrementInitiativeClicked(const QModelIndex& index);
        //cast row
        void spellNameEdited(const QUuid& id, const QString& name) const;
        void decrementCastingTimeClicked(const QUuid& id);
        void incrementCastingTimeClicked(const QUuid& id);
        void decrementDurationClicked(const QUuid& id);
        void incrementDurationClicked(const QUuid& id);
        void castSubmitClicked(const QUuid& id, const std::shared_ptr<Character>& character);
        // attack row
        void decrementAttackAmountClicked(const QModelIndex& index);
        void incrementAttackAmountClicked(const QModelIndex& index);
        void decrementWeaponSpeedClicked(const QModelIndex& index);
        void incrementWeaponSpeedClicked(const QModelIndex& index);
        void attackSubmitClicked(const QModelIndex& index);
        //misc row
        void miscSubmitClicked(const QModelIndex& index);
        void iconSelectorClicked(const QModelIndex& index, Cyrus::MiscActionType miscActionType);

    private:

        //--------- Helpers -----------
        static QVector<QRect> buildIconRects(const QRect& iconSelectorRect, 
                                        int iconCount, int padding);
        static AttackRects buildAttackRects(const QRect& dropdownRect,
                                                    const LayoutSpec& spec);
        static CastRects buildCastRects(const QRect& dropdownRect,
                                        const LayoutSpec& spec);
        static MiscRects buildMiscRects(const QRect& dropdownRect,
                                                const LayoutSpec& spec);

        static int minimumCastRowWidth(const Character& character);
        static int castingTimeLabelWidth();
        static int durationLabelWidth();
        static int attackAmountLabelWidth();
        static int weaponSpeedLabelWidth();

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

        void paintMiscDropdown(QPainter* painter,
                            const QRect& dropdownRect,
                            const MiscRects& cast,
                            const std::shared_ptr<Character>& character,
                            const QPoint& localCursor) const;

};   

    

