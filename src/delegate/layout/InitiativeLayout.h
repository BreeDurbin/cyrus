#pragma once
#include "Character.h"
#include "Enums.h"
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

    // Core API
    static InitiativeRects calculate(const QRect& rect,
                                     const Character::LayoutSpec& spec,
                                     const Character& character,
                                     bool isActiveIndex,
                                     bool isExpanded);

    static HitTestResult hitTest(const InitiativeRects& rects,
                                 const Character& character,
                                 const QPoint& cursorPos);
    
    static int preferredWidth(const Character::LayoutSpec& spec,
                              const std::shared_ptr<Character>& character);

private:
    // Helpers, all static
    static QRect buildIconSelectorRect(const QRect& deleteRect, int rowHeight,
                                       const Character::LayoutSpec& spec);
    static QVector<QRect> buildActionIconRects(const QRect& iconSelectorRect,
                                               int rowHeight,
                                               const Character::LayoutSpec& spec);
    static QRect buildMainRowRect(const QRect& base, bool isExpanded);
    static QRect buildDropdownRect(const QRect& baseRect);
    static StepperRects buildStepperRects(const QRect& frame,
                                          int buttonW, int valueW,
                                          int h, int padding);
    static CastRects buildCastRects(const QRect& dropdownRect,
                                    const Character::LayoutSpec& spec);

    static int preferredCastRowWidth(const Character::LayoutSpec& spec, const Character& character);

    static int castingTimeLabelWidth();

    static int durationLabelWidth();

};
