#include "RosterLayoutEngine.h"

#include<QRect>
#include<QPainter>

#include "LayoutUtils.h"
#include "IconRepository.h"
#include "ColorRepository.h"
#include "StyleRepository.h"
#include "PainterUtils.h"


std::shared_ptr<Layout> RosterLayoutEngine::calculate(
    const QRect& rect,
    const std::shared_ptr<Character>& character,
    bool isActiveIndex,
    bool isExpanded,
    const Cyrus::CombatSequenceState state
) const {
    auto spec = character->layoutSpec();
    int padding = spec.padding;

    auto layout = std::make_shared<RosterLayout>();
    layout->baseRect = LayoutUtils::padRectangle(rect, spec.padding);
    int rowHeight = layout->baseRect.height();

    LayoutUtils::LeftAnchorBuilder left(layout->baseRect);
    LayoutUtils::RightAnchorBuilder right(layout->baseRect);

    // Hero Icon
    layout->heroIconRect     = left.icon(spec.heroIconScale, padding);

    // delete button
    layout->deleteButtonRect = right.icon(spec.deleteButtonScale, padding);
    
    // clone button
    layout->cloneButtonRect  = right.icon(spec.cloneButtonScale, padding);

    // --- faction frame ---
    layout->factionFrame = left.control(rowHeight, rowHeight, padding);

    // --- build stepper rects using new API (changed) ---
    layout->factionStepperRects = Stepper::buildStepperRects(
        layout->factionFrame,
        rowHeight,
        0.6
    );

    // --- Text rect ---
    layout->textRect = left.text(layout->cloneButtonRect.left() - layout->factionFrame.right(), padding);

    return layout;
}


std::optional<HitCommand> RosterLayoutEngine::hitTest(
    const QModelIndex& index,
    const std::shared_ptr<Layout>& layout,
    const std::shared_ptr<Character>& character,
    const QPoint& cursorPos,
    const Cyrus::CombatSequenceState state) {
        
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

    // --- Faction stepper  ---
    if (rosterLayout->factionStepperRects.rightRect.contains(cursorPos)) { 
        return HitCommand{ [index, this](QListView* /*view*/) { emit nextFactionClicked(index); } };
    }
    if (rosterLayout->factionStepperRects.leftRect.contains(cursorPos)) { 
        return HitCommand{ [index, this](QListView* /*view*/) { emit previousFactionClicked(index); } };
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
    const QPoint& cursorPos,
    const Cyrus::CombatSequenceState state
) const {
    auto rosterLayout = std::dynamic_pointer_cast<RosterLayout>(layout);
    if (!rosterLayout) {
        return;
    }

    painter->save();
    auto spec = character->layoutSpec();

    // --- Background highlight ---
    if (isActiveIndex) {
        painter->fillRect(rosterLayout->baseRect, QColor(50, 50, 70, 128)); // soft active tint
    } else if (rosterLayout->baseRect.contains(cursorPos)) {
        painter->fillRect(rosterLayout->baseRect, QColor(80, 80, 100, 64));  // hover tint
    }

    // --- Hero icon ---
    QIcon heroIcon = character->icon();
    heroIcon.paint(painter, rosterLayout->heroIconRect, Qt::AlignCenter,
                   QIcon::Normal, QIcon::On);
    
    // --- Faction stepper 
    //StepperState hoverState = Stepper::buildHoverState(rosterLayout->factionStepperRects, cursorPos);
    StepperState hoverState;

    Stepper::paintStepper(
        painter,
        rosterLayout->factionStepperRects,
        hoverState,
        ColorRepository::colorForFaction(character->faction()),
        false
    );

    // --- Name ---
    painter->setFont(StyleRepository::textFont(14, true));
    painter->setPen(ColorRepository::text());
    painter->drawText(rosterLayout->textRect, Qt::AlignVCenter | Qt::AlignLeft, character->text());

    // --- Clone button ---
    PainterUtils::paintIcon(
        painter,
        IconRepository::clone_icon(),
        rosterLayout->cloneButtonRect,
        false,
        false,
        rosterLayout->cloneButtonRect.contains(cursorPos),
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
        rosterLayout->deleteButtonRect.contains(cursorPos),
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

    // Faction stepper 
    int controlHeight = static_cast<int>(spec.preferredHeight * 0.6); 
    int stepperWidth  = controlHeight; // vertical stepper is a square footprint 

    // Clone + delete buttons
    int buttonW = static_cast<int>(controlHeight * spec.deleteButtonScale) +
                  spec.padding;
    int cloneW  = buttonW;
    int deleteW = buttonW;

    // Text width
    int textW = 80; 

    int minWidth = heroW + spec.padding +
                   stepperWidth + spec.padding +
                   textW + spec.padding +
                   cloneW + deleteW;

    return minWidth;
}
