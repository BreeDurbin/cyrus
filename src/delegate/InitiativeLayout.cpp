#include "InitiativeLayout.h"
#include "style/IconRepository.h"
#include "style/StyleRepository.h"
#include <algorithm>

// Constructor
InitiativeLayout::InitiativeLayout(const Character::LayoutSpec& spec,
                                   const Character& character,
                                   bool isActiveIndex,
                                   bool isExpanded)
    : spec_(spec),
      character_(character),
      isActiveIndex_(isActiveIndex),
      isExpanded_(isExpanded) {}


// ----------------- Public API -----------------
InitiativeLayout::InitiativeRects
InitiativeLayout::calculate(const QRect& rect) const {
    InitiativeRects r;

    r.baseRect = padRectangle(rect);
    r.mainRowRect = buildMainRowRect(r.baseRect);
    r.dropdownRect = isExpanded_ ? buildDropdownRect(r.baseRect) : QRect();

    r.heroIconRect = buildHeroIconRect(r.mainRowRect);
    r.initiativeRect = buildInitiativeRect(r.heroIconRect);
    r.deleteRect = buildDeleteRect(r.mainRowRect);
    r.iconSelectorRect = isActiveIndex_ ? buildIconSelectorRect(r.deleteRect, r.mainRowRect.height()) : QRect();
    r.rightAnchorRect = isActiveIndex_ ? r.iconSelectorRect : r.deleteRect;
    r.textRect = buildTextRect(r.initiativeRect, r.rightAnchorRect);

    if (isActiveIndex_) {
        r.actionIconRects = buildActionIconRects(r.iconSelectorRect, r.mainRowRect.height());
    }

    if (isExpanded_ && character_.actionType() == Cyrus::ActionType::Cast) {
        QFont f = StyleRepository::labelFont(14, false);
        QFontMetrics fm(f);
        r.cast = buildCastRects(r.dropdownRect);
    }

    return r;
}

HitTestResult InitiativeLayout::hitTest(const InitiativeRects& rects,
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



// ----------------- Helpers -----------------
QRect InitiativeLayout::padRectangle(const QRect& base) const {
    int halfPadding = spec_.padding / 2;
    return base.adjusted(halfPadding, halfPadding, -halfPadding, -halfPadding);
}

QRect InitiativeLayout::buildHeroIconRect(const QRect& base) const {
    const int heroIconSide = static_cast<int>(base.height() * spec_.heroIconScale);
    const QSize heroIconSize(heroIconSide, heroIconSide);
    const int leftOffset = spec_.padding;

    return QRect(base.left() + leftOffset,
                 base.top() + (base.height() - heroIconSize.height()) / 2,
                 heroIconSize.width(), heroIconSize.height());
}

QRect InitiativeLayout::buildInitiativeRect(const QRect& left) const {
    return QRect(left.right() + spec_.padding, left.top(),
                 spec_.initiativeWidth, left.height());
}

QRect InitiativeLayout::buildTextRect(const QRect& left, const QRect& right) const {
    return QRect(left.right() + spec_.padding, left.top(),
                 right.left() - left.right() - (2 * spec_.padding),
                 left.height());
}

QRect InitiativeLayout::buildDeleteRect(const QRect& base) const {
    const int rectSize = static_cast<int>(base.height() * 0.60);
    return QRect(base.right() - rectSize - spec_.padding,
                 base.top() + (base.height() - rectSize) / 2,
                 rectSize, rectSize);
}

QRect InitiativeLayout::buildIconSelectorRect(const QRect& deleteRect, int rowHeight) const {
    const QList<QIcon> icons = IconRepository::actionIcons();
    if (icons.isEmpty()) return QRect();

    const int iconCount = icons.size();
    const int iconSize = static_cast<int>(rowHeight * spec_.iconSelectorIconScale);
    const int totalWidth = iconCount * iconSize + (iconCount - 1) * spec_.padding;

    return QRect(deleteRect.left() - totalWidth - spec_.padding,
                 deleteRect.top() + (deleteRect.height() - iconSize) / 2,
                 totalWidth, iconSize);
}

QVector<QRect> InitiativeLayout::buildActionIconRects(const QRect& iconSelectorRect, int rowHeight) const {
    QVector<QRect> rects;
    const int iconSize = static_cast<int>(rowHeight * spec_.iconSelectorIconScale);

    int x = iconSelectorRect.left();
    for (auto actionType : Cyrus::orderedActionTypes) {
        rects << QRect(x,
                       iconSelectorRect.top() + (iconSelectorRect.height() - iconSize) / 2,
                       iconSize, iconSize);
        x += iconSize + spec_.padding;
    }
    return rects;
}

QRect InitiativeLayout::buildMainRowRect(const QRect& base) const {
    return isExpanded_ ? base.adjusted(0, 0, 0, -base.height() / 2) : base;
}

QRect InitiativeLayout::buildDropdownRect(const QRect& baseRect) const {
    return baseRect.adjusted(0, baseRect.height() / 2, 0, 0);
}


// ----------------- Stepper + Cast -----------------
InitiativeLayout::StepperRects
InitiativeLayout::buildStepperRects(const QRect& frame, int buttonW, int valueW, int h, int padding) const {
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
InitiativeLayout::buildCastRects(const QRect& dropdownRect) const {
    CastRects c;

    const int controlHeight      = 24;
    const int stepperButtonWidth = 22;
    const int stepperValueWidth  = 40;
    const int padding            = spec_.padding / 2;

    int xPos      = dropdownRect.left() + (padding * 2);
    int yPos      = dropdownRect.top();
    int rowHeight = dropdownRect.height();

    QFont f = StyleRepository::labelFont(14, false);
    QFontMetrics fm(f);

    const int castingTimeLabelWidth = fm.horizontalAdvance(QStringLiteral("Speed")) + 6;
    const int durationLabelWidth    = fm.horizontalAdvance(QStringLiteral("Dur"))   + 6;

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
