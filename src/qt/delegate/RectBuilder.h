#include "../model/Character.h"          // for layoutSpec
#include "../helper/Enums.h"
#include <QRect>
#include <QIcon>
#include <QList>

namespace RectBuilder {
    using LayoutSpec = Character::LayoutSpec;


    // Pill stepper rects
    struct StepperRects {
        QRect minusRect;
        QRect valueRect;
        QRect plusRect;
    };

    inline StepperRects buildStepperRects(const QRect& frame, int buttonW, int valueW, int h, int padding) {
        StepperRects s;
        int x = frame.left();
        int y = frame.top() + (frame.height() - h) / 2;

        s.minusRect = QRect(x, y, buttonW, h);
        x += buttonW + padding;

        s.valueRect = QRect(x, y, valueW, h);
        x += valueW + padding;

        s.plusRect  = QRect(x, y, buttonW, h);
        return s;
    }

    //Build rects for dropdown rects Cast case
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

    inline CastRects buildCastRects(const QRect& dropdownRect,
                                    const Character::LayoutSpec& spec,
                                    const QFontMetrics& fm) {
        CastRects c;

        // Control sizing constants
        const int controlHeight      = 24;
        const int stepperButtonWidth = 22;
        const int stepperValueWidth  = 40;
        const int padding            = spec.padding / 2;

        // Line metrics
        int xPos      = dropdownRect.left() + (padding * 2);
        int yPos      = dropdownRect.top();
        int rowHeight = dropdownRect.height();

        // Label widths (text metrics + small buffer)
        const int castingTimeLabelWidth = fm.horizontalAdvance(QStringLiteral("Speed")) + 6;
        const int durationLabelWidth    = fm.horizontalAdvance(QStringLiteral("Dur"))   + 6;

        // Fixed widths for steppers and OK button
        const int stepperWidth = stepperButtonWidth + padding +
                                stepperValueWidth + padding +
                                stepperButtonWidth;
        const int okButtonSize = 28;

        // Available width for spell edit = total - reserved (steppers + labels + OK + padding)
        int reservedWidth =
            (castingTimeLabelWidth + padding + stepperWidth + (padding * 2)) +
            (durationLabelWidth    + padding + stepperWidth + (padding * 2)) +
            (okButtonSize + (padding * 4)); // include both left + right padding

        int spellEditWidth = std::max(100, dropdownRect.width() - reservedWidth - (padding * 2));

        // Spell edit field (acts as both label+input)
        c.spellEdit = QRect(xPos, yPos + (rowHeight - controlHeight) / 2,
                            spellEditWidth, controlHeight);
        xPos += spellEditWidth + (padding * 2);

        // Casting Time label + stepper
        c.castingTimeLabel = QRect(xPos, yPos + (rowHeight - controlHeight) / 2,
                                castingTimeLabelWidth, controlHeight);
        xPos += castingTimeLabelWidth + padding;

        c.castingTimeFrame = QRect(xPos, yPos, stepperWidth, rowHeight);
        c.castingTimeStepperRects = buildStepperRects(c.castingTimeFrame,
                                                    stepperButtonWidth,
                                                    stepperValueWidth,
                                                    controlHeight,
                                                    padding);
        xPos += stepperWidth + (padding * 2);

        // Duration label + stepper
        c.durationLabel = QRect(xPos, yPos + (rowHeight - controlHeight) / 2,
                                durationLabelWidth, controlHeight);
        xPos += durationLabelWidth + padding;

        c.durationFrame = QRect(xPos, yPos, stepperWidth, rowHeight);
        c.durationStepperRects = buildStepperRects(c.durationFrame,
                                                stepperButtonWidth,
                                                stepperValueWidth,
                                                controlHeight,
                                                padding);
        xPos += stepperWidth + (padding * 2);

        // OK button
        c.okRect = QRect(xPos,
                        yPos + (rowHeight - okButtonSize) / 2,
                        okButtonSize, okButtonSize);

        return c;
    }




    //MAIN BOX RECTS
    // Pad a rectangle evenly on all sides
    inline QRect padRectangle(const QRect& base, int padding) {
        int halfPadding = padding / 2;
        return base.adjusted(halfPadding, halfPadding, -halfPadding, -halfPadding);
    }

    // Hero icon rectangle
    inline QRect buildHeroIconRect(const QRect& base, const LayoutSpec& spec) {
        const int heroIconSide = static_cast<int>(base.height() * spec.heroIconScale);
        const QSize heroIconSize(heroIconSide, heroIconSide);
        const int leftOffset = spec.padding;

        return QRect(base.left() + leftOffset,
                    base.top() + (base.height() - heroIconSize.height()) / 2,
                    heroIconSize.width(), heroIconSize.height());
    }

    // Initiative rectangle (next to hero icon)
    inline QRect buildInitiativeRect(const QRect& left, const LayoutSpec& spec) {
        const int spacing = spec.padding;
        const int initiativeWidth = spec.initiativeWidth;

        return QRect(left.right() + spacing, left.top(),
                    initiativeWidth, left.height());
    }

    // Text rectangle between initiative and right-side element
    inline QRect buildTextRect(const QRect& left, const QRect& right, const LayoutSpec& spec) {
        const int spacing = spec.padding;

        return QRect(left.right() + spacing, left.top(),
                    right.left() - left.right() - (2 * spacing),
                    left.height());
    }

    // Icon selector rectangle (action buttons)
    inline QRect buildIconSelectorRect(const QRect& deleteRect, const LayoutSpec& spec, int rowHeight) {
        const QList<QIcon> icons = IconRepository::actionIcons();
        if (icons.isEmpty()) return QRect();

        const int iconCount = icons.size();
        const int iconSize = static_cast<int>(rowHeight * spec.iconSelectorIconScale);
        const int totalWidth = iconCount * iconSize + (iconCount - 1) * spec.padding;

        return QRect(deleteRect.left() - totalWidth - spec.padding,
                    deleteRect.top() + (deleteRect.height() - iconSize) / 2,
                    totalWidth, iconSize);
    }

    // Delete button rectangle
    inline QRect buildDeleteRect(const QRect& base, const LayoutSpec& spec) {
        const int spacing = spec.padding;
        const int rectSize = static_cast<int>(base.height() * 0.60);

        return QRect(base.right() - rectSize - spacing,
                    base.top() + (base.height() - rectSize) / 2,
                    rectSize, rectSize);
    }

    // New: top half (main row) of an expanded item.
    // If not expanded, just return the full base.
    inline QRect buildMainRowRect(const QRect& base, bool expanded) {
        return expanded ? base.adjusted(0, 0, 0, -base.height() / 2) : base;
    }

    // Dropdown Rect
    inline QRect buildDropdownRect(const QRect& baseRect) {
        return baseRect.adjusted(0, baseRect.height() / 2, 0, 0);
    }

    inline QVector<QRect> buildActionIconRects(const QRect &iconSelectorRect, const Character::LayoutSpec &spec, int rowHeight) {
        QVector<QRect> rects;
        const int iconSize = static_cast<int>(rowHeight * spec.iconSelectorIconScale);

        int x = iconSelectorRect.left();

        for (auto actionType : Cyrus::orderedActionTypes) {
            rects << QRect(x,
                        iconSelectorRect.top() + (iconSelectorRect.height() - iconSize) / 2,
                        iconSize, iconSize);
            x += iconSize + spec.padding;
        }

        return rects;
    }

    // Helper to duild initiative view rects consistently across diff functions paint, edit etc
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

    inline InitiativeRects calculateRects(
        const QRect& rect,
        const Character& character,
        bool isActiveIndex,
        bool isExpanded) {
        InitiativeRects r;

        const LayoutSpec& spec = character.layoutSpec();
        r.baseRect = padRectangle(rect, spec.padding);

        r.mainRowRect = RectBuilder::buildMainRowRect(r.baseRect, isExpanded);
        r.dropdownRect = isExpanded ? RectBuilder::buildDropdownRect(r.baseRect) : QRect();

        r.heroIconRect = buildHeroIconRect(r.mainRowRect, spec);
        r.initiativeRect = buildInitiativeRect(r.heroIconRect, spec);
        r.deleteRect = buildDeleteRect(r.mainRowRect, spec);
        r.iconSelectorRect = isActiveIndex ? buildIconSelectorRect(r.deleteRect, spec, r.mainRowRect.height()) : QRect();
        r.rightAnchorRect = isActiveIndex ? r.iconSelectorRect : r.deleteRect;
        r.textRect = buildTextRect(r.initiativeRect, r.rightAnchorRect, spec);

        if (isActiveIndex) {
            r.actionIconRects = buildActionIconRects(r.iconSelectorRect, spec, r.mainRowRect.height());
        }

        if (isExpanded && character.actionType() == Cyrus::ActionType::Cast) {
            QFont f = StyleRepository::labelFont(14, false);
            QFontMetrics fm(f);
            r.cast = buildCastRects(r.dropdownRect, spec, fm);
        }
        
        return r;
    }
}