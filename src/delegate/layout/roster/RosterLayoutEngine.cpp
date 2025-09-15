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
#include <QApplication>


std::shared_ptr<Layout> RosterLayoutEngine::calculate(
    const QRect& rect,
    const std::shared_ptr<Character>& character,
    bool isActiveIndex,
    bool isExpanded) const
{
    auto spec = character->layoutSpec();
    int padding = spec.padding;

    auto layout = std::make_shared<RosterLayout>();
    layout->baseRect = LayoutUtils::padRectangle(rect, spec.padding);
    int rowHeight = layout->baseRect.height();

    // Local vars
    int controlHeight       = static_cast<int>(layout->baseRect.height() * 0.6);   // 60% of row
    int stepperButtonWidth  = controlHeight;                                    // square buttons
    int stepperValueWidth   = static_cast<int>(controlHeight * 1.5);            // wider value
    int stepperWidth        = stepperButtonWidth + spec.padding +
                              stepperValueWidth + spec.padding +
                              stepperButtonWidth;

    LayoutUtils::LeftAnchorBuilder left(layout->baseRect);
    LayoutUtils::RightAnchorBuilder right(layout->baseRect);

    //Hero Icon
    layout->heroIconRect     = left.icon(spec.heroIconScale, padding);

    // delete button
    layout->deleteButtonRect = right.icon(spec.deleteButtonScale, padding);
    
    // clone button
    layout->cloneButtonRect  = right.icon(spec.cloneButtonScale, padding);

    layout->initiativeFrame = left.control(stepperWidth, rowHeight, padding);

    layout->initiativeStepperRects = Stepper::buildStepperRects(
        layout->initiativeFrame,
        stepperButtonWidth,
        stepperValueWidth,
        controlHeight,
        spec.padding
    );

    // --- Text rect ---
    layout->textRect = left.text(layout->cloneButtonRect.left() - layout->initiativeFrame.right(), padding);

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
        return HitCommand{ [index, this](QListView* /*view*/) { emit deleteButtonClicked(index); } };
    }

    // --- Clone button ---
    if (rosterLayout->cloneButtonRect.contains(cursorPos)) {
        return HitCommand{ [index, this](QListView* /*view*/) { emit cloneRosterMemberClicked(index); } };
    }

    // --- Initiative stepper ---
    if (rosterLayout->initiativeStepperRects.plusRect.contains(cursorPos)) {
        return HitCommand{ [index, this](QListView* /*view*/) { emit incrementRosterMemberInitiativeClicked(index); } };
    }
    if (rosterLayout->initiativeStepperRects.minusRect.contains(cursorPos)) {
        return HitCommand{ [index, this](QListView* /*view*/) { emit decrementRosterMemberInitiativeClicked(index); } };
    }

    return std::nullopt;
}


void RosterLayoutEngine::paintLayout(
    QPainter* painter,
    const std::shared_ptr<Layout>& layout,
    const std::shared_ptr<Character>& character,
    const CastState castState,
    bool isActiveIndex,
    bool isExpanded,
    const QPoint& localCursor) const
{
    auto rosterLayout = std::dynamic_pointer_cast<RosterLayout>(layout);
    if (!rosterLayout) {
        return;
    }

    painter->save();
    auto spec = character->layoutSpec();

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
    Stepper::paintStepper(
        painter,
        rosterLayout->initiativeStepperRects,
        character->initiative()
    );

    // --- Name ---
    painter->setFont(StyleRepository::labelFont(14, true));
    painter->setPen(ColorRepository::text());
    painter->drawText(rosterLayout->textRect, Qt::AlignVCenter | Qt::AlignLeft, character->text());

    // --- Clone button ---
    PainterUtils::paintIcon(
        painter,
        IconRepository::clone_icon(),
        rosterLayout->cloneButtonRect,
        false,
        false,
        rosterLayout->cloneButtonRect.contains(localCursor),
        rosterLayout->cloneButtonRect.height()
    );

    // --- Delete button ---
    const int deleteIconSize = static_cast<int>(rosterLayout->baseRect.height() * spec.deleteButtonScale);
    PainterUtils::paintIcon(
        painter,
        IconRepository::delete_icon(),
        rosterLayout->deleteButtonRect,
        false,
        false,
        rosterLayout->deleteButtonRect.contains(localCursor),
        rosterLayout->deleteButtonRect.height()
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
