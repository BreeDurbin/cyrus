#pragma once
#include "model/Character.h"
#include "style/Enums.h"
#include <QRect>
#include <QVector>
#include <QFontMetrics>

class InitiativeLayout {
public:
    struct StepperRects {
        QRect minusRect;
        QRect valueRect;
        QRect plusRect;
    };

    struct CastRects {
        QRect spellEdit;
        QRect castingTimeLabel;
        QRect castingTimeFrame;
        StepperRects castingTimeStepperRects;
        QRect durationLabel;
        QRect durationFrame;
        StepperRects durationStepperRects;
        QRect okRect;
    };

    struct InitiativeRects {
        QRect baseRect;
        QRect mainRowRect;
        QRect dropdownRect;
        QRect heroIconRect;
        QRect initiativeRect;
        QRect deleteRect;
        QRect iconSelectorRect;
        QRect rightAnchorRect;
        QRect textRect;
        QVector<QRect> actionIconRects;
        CastRects cast;
    };

    struct HitTestResult {
        enum Type {
            None,
            DeleteButton,
            ActionIcon,
            SpellEdit,
            CastingTimeMinus,
            CastingTimePlus,
            DurationMinus,
            DurationPlus,
            CastOk
        };
        Type type = None;
        int actionIconIndex = -1; // valid if Type == ActionIcon
    };

    InitiativeLayout(const Character::LayoutSpec& spec,
                     const Character& character,
                     bool isActiveIndex,
                     bool isExpanded);

    static InitiativeRects calculate(const QRect& rect) const;

    static HitTestResult hitTest(const InitiativeRects& rects,
                                 const Character& character,
                                 const QPoint& cursorPos);

private:
    Character::LayoutSpec spec_;
    const Character& character_;
    bool isActiveIndex_;
    bool isExpanded_;

    // Helpers
    QRect padRectangle(const QRect& base) const;
    QRect buildHeroIconRect(const QRect& base) const;
    QRect buildInitiativeRect(const QRect& left) const;
    QRect buildTextRect(const QRect& left, const QRect& right) const;
    QRect buildDeleteRect(const QRect& base) const;
    QRect buildIconSelectorRect(const QRect& deleteRect, int rowHeight) const;
    QVector<QRect> buildActionIconRects(const QRect& iconSelectorRect, int rowHeight) const;
    QRect buildMainRowRect(const QRect& base) const;
    QRect buildDropdownRect(const QRect& baseRect) const;
    StepperRects buildStepperRects(const QRect& frame, int buttonW, int valueW, int h, int padding) const;
    CastRects buildCastRects(const QRect& dropdownRect) const;
};
