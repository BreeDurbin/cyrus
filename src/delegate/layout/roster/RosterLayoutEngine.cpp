#include "RosterLayoutEngine.h"

#include <memory>
#include <QRect>
#include <QModelIndex>
#include "LayoutUtils.h"
#include "Stepper.h"
#include "RosterLayoutEngine.h"
#include "PainterUtils.h"
#include "Stepper.h"
#include "StyleRepository.h"
#include "ColorRepository.h"


std::shared_ptr<Layout> RosterLayoutEngine::calculate(
    const QRect& rect,
    const std::shared_ptr<Character>& character,
    bool isActiveIndex,
    bool isExpanded) const
{
    auto spec = character->layoutSpec();

    auto layout = std::make_shared<RosterLayout>();
    layout->baseRect = LayoutUtils::padRectangle(rect, spec.padding);

    // --- Hero icon ---
    layout->heroIconRect = LayoutUtils::buildHeroIconRect(layout->baseRect, spec.heroIconScale);
    layout->heroIconRect.moveTop(layout->baseRect.top() +
                              (layout->baseRect.height() - layout->heroIconRect.height()) / 2);

    // --- Initiative stepper ---
    int controlHeight       = static_cast<int>(layout->baseRect.height() * 0.6);   // 60% of row
    int stepperButtonWidth  = controlHeight;                                    // square buttons
    int stepperValueWidth   = static_cast<int>(controlHeight * 1.5);            // wider value
    int stepperWidth        = stepperButtonWidth + spec.padding +
                              stepperValueWidth + spec.padding +
                              stepperButtonWidth;

    layout->initiativeFrame = LayoutUtils::buildInitiativeRect(layout->heroIconRect,
                                                           spec.padding,
                                                           stepperWidth);
    layout->initiativeFrame.setTop(layout->baseRect.top() +
                                (layout->baseRect.height() - controlHeight) / 2);
    layout->initiativeFrame.setHeight(controlHeight);

    layout->initiativeStepperRects = Stepper::buildStepperRects(
        layout->initiativeFrame,
        stepperButtonWidth,
        stepperValueWidth,
        controlHeight,
        spec.padding
    );

    // --- Delete button ---
    layout->deleteButtonRect = LayoutUtils::buildDeleteRect(layout->baseRect,
                                                        spec.padding,
                                                        spec.deleteButtonScale);
    layout->deleteButtonRect.moveTop(layout->baseRect.top() +
                                  (layout->baseRect.height() - layout->deleteButtonRect.height()) / 2);

    // --- Clone button ---
    layout->cloneButtonRect = layout->deleteButtonRect;
    layout->cloneButtonRect.moveLeft(layout->deleteButtonRect.left() - spec.padding - layout->cloneButtonRect.width());

    // --- Text rect ---
    layout->textRect = LayoutUtils::buildTextRect(layout->initiativeFrame,
                                               layout->cloneButtonRect,
                                               spec.padding);

    return layout;
}


std::optional<HitCommand> RosterLayoutEngine::hitTest(
    const QModelIndex& index,
    const std::shared_ptr<Layout>& layout,
    const std::shared_ptr<Character>& character,
    const QPoint& cursorPos)
{
    auto rosterLayout = std::dynamic_pointer_cast<RosterLayout>(layout);
    if (!rosterLayout) {
        return std::nullopt;
    }

    // --- Delete button ---
    if (rosterLayout->deleteButtonRect.contains(cursorPos)) {
        return HitCommand{ [index, this]() { emit deleteButtonClicked(index); } };
    }

    // --- Clone button ---
    if (rosterLayout->cloneButtonRect.contains(cursorPos)) {
        return HitCommand{ [index, this]() { emit cloneRosterMemberClicked(index); } };
    }

    // --- Initiative stepper ---
    if (rosterLayout->initiativeStepperRects.plusRect.contains(cursorPos)) {
        return HitCommand{ [index, this]() { emit incrementRosterMemberInitiativeClicked(index); } };
    }
    if (rosterLayout->initiativeStepperRects.minusRect.contains(cursorPos)) {
        return HitCommand{ [index, this]() { emit decrementRosterMemberInitiativeClicked(index); } };
    }

    return std::nullopt;
}


void RosterLayoutEngine::paintLayout(
    QPainter* painter,
    const std::shared_ptr<Layout>& layout,
    const std::shared_ptr<Character>& character,
    const CastState& castState,
    bool isActiveIndex,
    bool isExpanded,
    const QPoint& localCursor) const
{
    auto rosterLayout = std::dynamic_pointer_cast<RosterLayout>(layout);
    if (!rosterLayout) {
        return;
    }

    painter->save();

    // --- Background highlight ---
    if (isActiveIndex) {
        painter->fillRect(rosterLayout->baseRect, QColor(50, 50, 70, 128)); // soft active tint
    } else if (rosterLayout->baseRect.contains(localCursor)) {
        painter->fillRect(rosterLayout->baseRect, QColor(80, 80, 100, 64));  // hover tint
    }

    // --- Hero icon ---
    QIcon heroIcon = character->icon();
    heroIcon.paint(painter, rosterLayout->heroIconRect, Qt::AlignCenter,
                   QIcon::Normal, QIcon::On);
    
    // --- Initiative stepper ---
    PainterUtils::paintStepper(
        painter,
        rosterLayout->initiativeStepperRects,
        character->initiative()
    );

    // --- Name ---
    painter->setFont(StyleRepository::labelFont(14, true));
    painter->setPen(ColorRepository::text());
    painter->drawText(rosterLayout->textRect, Qt::AlignVCenter | Qt::AlignLeft,
                      character->text());


    // --- Clone button ---
    QIcon cloneIcon = IconRepository::clone();
    cloneIcon.paint(painter, rosterLayout->cloneButtonRect);

    // --- Delete button ---
    PainterUtils::paintDeleteButton(
        painter,
        rosterLayout->deleteButtonRect,
        rosterLayout->deleteButtonRect.contains(localCursor)
    );

    painter->restore();
}

int RosterLayoutEngine::minimumWidth(
    const std::shared_ptr<Character>& character) const
{
    auto spec = character->layoutSpec();

    // Hero icon width
    int heroW = static_cast<int>(spec.heroIconScale * spec.preferredHeight);

    // Initiative stepper widths
    int controlHeight      = static_cast<int>(spec.preferredHeight * 0.6);
    int stepperButtonWidth = controlHeight;                  // square
    int stepperValueWidth  = static_cast<int>(controlHeight * 1.5);
    int stepperWidth       = stepperButtonWidth + spec.padding +
                             stepperValueWidth + spec.padding +
                             stepperButtonWidth;

    // Clone + delete buttons (same size)
    int buttonW = static_cast<int>(controlHeight * spec.deleteButtonScale) +
                  spec.padding; // include padding
    int cloneW  = buttonW;
    int deleteW = buttonW;

    // Text width: minimum fallback (at least some space for name)
    int textW = 80; // arbitrary baseline; you can compute from font metrics if needed

    // Put it all together
    int minWidth = heroW + spec.padding +
                   stepperWidth + spec.padding +
                   textW + spec.padding +
                   cloneW + deleteW;

    return minWidth;
}
