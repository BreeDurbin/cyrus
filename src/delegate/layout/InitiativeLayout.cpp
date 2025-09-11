#include "InitiativeLayout.h"
#include "IconRepository.h"
#include "StyleRepository.h"
#include "LayoutUtils.h"

#include <algorithm>
#include "InitiativeLayout.h"
#include "IconRepository.h"
#include "StyleRepository.h"
#include "LayoutUtils.h"

#include <algorithm>

// ----------------- Public API -----------------
InitiativeLayout::InitiativeRects
InitiativeLayout::calculate(const QRect& rect,
                            const Character::LayoutSpec& spec,
                            const Character& character,
                            bool isActiveIndex,
                            bool isExpanded)
{
    InitiativeRects r;

    // base + rows
    r.baseRect     = LayoutUtils::padRectangle(rect, spec.padding);
    r.mainRowRect  = buildMainRowRect(r.baseRect, isExpanded);
    r.dropdownRect = isExpanded ? buildDropdownRect(r.baseRect) : QRect();

    // icon + controls
    r.heroIconRect   = LayoutUtils::buildHeroIconRect(r.mainRowRect, spec.heroIconScale);
    r.initiativeRect = LayoutUtils::buildInitiativeRect(r.heroIconRect, spec.padding, spec.initiativeWidth);
    r.deleteRect     = LayoutUtils::buildDeleteRect(r.mainRowRect, spec.padding);

    // right-hand controls
    r.iconSelectorRect = isActiveIndex
        ? buildIconSelectorRect(r.deleteRect, r.mainRowRect.height(), spec)
        : QRect();
    r.rightAnchorRect = isActiveIndex ? r.iconSelectorRect : r.deleteRect;

    // text
    r.textRect = LayoutUtils::buildTextRect(r.initiativeRect, r.rightAnchorRect, spec.padding);

    // action icons
    if (isActiveIndex) {
        r.actionIconRects = buildActionIconRects(r.iconSelectorRect, r.mainRowRect.height(), spec);
    }

    // cast dropdown
    if (isExpanded && character.actionType() == Cyrus::ActionType::Cast) {
        r.cast = buildCastRects(r.dropdownRect, spec);
    }

    return r;
}

InitiativeLayout::HitTestResult
InitiativeLayout::hitTest(const InitiativeRects& rects,
                          const Character& character,
                          const QPoint& cursorPos)
{
    HitTestResult result;

    if (rects.deleteRect.contains(cursorPos)) {
        result.type = HitTestResult::DeleteButton;
        return result;
    }

    for (int i = 0; i < rects.actionIconRects.size(); ++i) {
        if (rects.actionIconRects[i].contains(cursorPos)) {
            result.type = HitTestResult::ActionIcon;
            result.actionIconIndex = i;
            return result;
        }
    }

    if (character.actionType() == Cyrus::ActionType::Cast) {
        if (rects.cast.spellEdit.contains(cursorPos)) {
            result.type = HitTestResult::SpellEdit;
            return result;
        }
        if (rects.cast.castingTimeStepperRects.minusRect.contains(cursorPos)) {
            result.type = HitTestResult::CastingTimeMinus;
            return result;
        }
        if (rects.cast.castingTimeStepperRects.plusRect.contains(cursorPos)) {
            result.type = HitTestResult::CastingTimePlus;
            return result;
        }
        if (rects.cast.durationStepperRects.minusRect.contains(cursorPos)) {
            result.type = HitTestResult::DurationMinus;
            return result;
        }
        if (rects.cast.durationStepperRects.plusRect.contains(cursorPos)) {
            result.type = HitTestResult::DurationPlus;
            return result;
        }
        if (rects.cast.okRect.contains(cursorPos)) {
            result.type = HitTestResult::CastOk;
            return result;
        }
    }

    return result;
}

int InitiativeLayout::preferredWidth(const Character::LayoutSpec& spec,
                                     const std::shared_ptr<Character>& character) {
    return preferredCastRowWidth(spec, *character);
}

// ----------------- Icon selector + rows -----------------
QRect InitiativeLayout::buildIconSelectorRect(const QRect& deleteRect, int rowHeight,
                                              const Character::LayoutSpec& spec) {
    const QList<QIcon> icons = IconRepository::actionIcons();
    if (icons.isEmpty()) return QRect();

    const int iconCount = icons.size();
    const int iconSize  = static_cast<int>(rowHeight * spec.iconSelectorIconScale);
    const int totalW    = iconCount * iconSize + (iconCount - 1) * spec.padding;

    return QRect(deleteRect.left() - totalW - spec.padding,
                 deleteRect.top() + (deleteRect.height() - iconSize) / 2,
                 totalW, iconSize);
}

QVector<QRect>
InitiativeLayout::buildActionIconRects(const QRect& iconSelectorRect,
                                       int rowHeight,
                                       const Character::LayoutSpec& spec) {
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

QRect InitiativeLayout::buildMainRowRect(const QRect& base, bool isExpanded) {
    return isExpanded ? base.adjusted(0, 0, 0, -base.height() / 2) : base;
}

//to do work out spacing between itemss sin teh top row and bottom row of expanded rect
QRect InitiativeLayout::buildDropdownRect(const QRect& baseRect) {
    return baseRect.adjusted(0, baseRect.height() / 2, 0, 0);
}

// ----------------- Stepper + Cast -----------------
InitiativeLayout::StepperRects
InitiativeLayout::buildStepperRects(const QRect& frame,
                                    int buttonW, int valueW,
                                    int h, int padding) {
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

InitiativeLayout::CastRects
InitiativeLayout::buildCastRects(const QRect& dropdownRect,
                                 const Character::LayoutSpec& spec) {
    CastRects c;

    const int controlHeight      = 24;
    const int stepperButtonWidth = 22;
    const int stepperValueWidth  = 40;
    const int padding            = spec.padding / 2;

    int xPos      = dropdownRect.left() + (padding * 2);
    int yPos      = dropdownRect.top();
    int rowHeight = dropdownRect.height();

    const int castingTimeLabelWidth = InitiativeLayout::castingTimeLabelWidth();
    const int durationLabelWidth    = InitiativeLayout::durationLabelWidth();

    const int stepperWidth = stepperButtonWidth + padding +
                             stepperValueWidth + padding +
                             stepperButtonWidth;
    const int okButtonSize = 28;

    int reservedWidth =
        (castingTimeLabelWidth + padding + stepperWidth + (padding * 2)) +
        (durationLabelWidth    + padding + stepperWidth + (padding * 2)) +
        (okButtonSize + (padding * 4));

    int spellEditWidth = std::max(100, dropdownRect.width() - reservedWidth - (padding * 2));

    // Spell edit
    c.spellEdit = QRect(xPos, yPos + (rowHeight - controlHeight) / 2,
                        spellEditWidth, controlHeight);
    xPos += spellEditWidth + (padding * 2);

    // Casting time
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

    // Duration
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


// Calculate the minimal width of the cast row

int InitiativeLayout::preferredCastRowWidth(const Character::LayoutSpec& spec,
                                            const Character& character)
{
    const int controlHeight      = 24;
    const int stepperButtonWidth = 22;
    const int stepperValueWidth  = 40;
    const int padding            = spec.padding / 2;

    const int castingTimeLabelW = InitiativeLayout::castingTimeLabelWidth();
    const int durationLabelW    = InitiativeLayout::durationLabelWidth();

    const int stepperWidth =
        stepperButtonWidth + padding +
        stepperValueWidth  + padding +
        stepperButtonWidth;

    const int okButtonSize   = 28;
    const int minSpellEditW  = 100; // minimum spell edit width

    // Build up required width
    int width = 0;

    // spell edit
    width += minSpellEditW + (padding * 2);

    // casting time label + stepper
    width += castingTimeLabelW + padding + stepperWidth + (padding * 2);

    // duration label + stepper
    width += durationLabelW + padding + stepperWidth + (padding * 2);

    // ok button
    width += okButtonSize + (padding * 4);

    // outer margins
    // item outer padding, left + right from LayoutEngine.padRectangle()
    width +=  2 * padding; 

    return width;
}



int InitiativeLayout::castingTimeLabelWidth(){
    QFont f = StyleRepository::labelFont(14, false);
    QFontMetrics fm(f);
    return fm.horizontalAdvance(QStringLiteral("Speed")) + 6;
}

int InitiativeLayout::durationLabelWidth(){
    QFont f = StyleRepository::labelFont(14, false);
    QFontMetrics fm(f);
    return fm.horizontalAdvance(QStringLiteral("Dur")) + 6;
}
