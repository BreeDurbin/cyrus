#include "CharacterLayoutEngine.h"

#include "IconRepository.h"
#include "StyleRepository.h"
#include "LayoutUtils.h"

#include <algorithm>
#include "IconRepository.h"
#include "ColorRepository.h"
#include <QPainter>
#include "PainterUtils.h"
#include "InitiativeView.h"
#include <QApplication>

// ----------------- Public API -----------------

std::shared_ptr<Layout> CharacterLayoutEngine::calculate(
    const QRect& rect,
    const std::shared_ptr<Character>& character,
    bool isSelectedIndex,
    bool isExpanded,
    const Cyrus::CombatSequenceState state
) const {
    CharacterLayout layout;
    auto spec = character->layoutSpec();
    int padding = spec.padding;

    // --- base + rows ---
    layout.baseRect     = LayoutUtils::padRectangle(rect, padding); //unpadded, used to build the top and bottom row rects
    layout.mainRowRect  = LayoutUtils::buildMainRowRect(rect, isExpanded, padding);
    layout.dropdownRect = isExpanded ? LayoutUtils::buildDropdownRect(rect, layout.mainRowRect, padding) : QRect();

    int rowHeight = layout.mainRowRect.height();

    // --- left-hand controls (hero + initiative) ---
    LayoutUtils::LeftAnchorBuilder left(layout.mainRowRect);
    layout.top.heroIconRect   = left.icon(spec.heroIconScale, padding);
    layout.top.initiativeIconRect = left.icon(spec.initiativeIconScale, padding);

    // build initaitve as a stepper if ininitiative combat state otherwise its a text box
    if (state == Cyrus::CombatSequenceState::INITIATIVE) {
        // Local vars
        const int stepperButtonWidth = 22;
        const int stepperValueWidth  = 40;
        const int padding            = spec.padding;
        const int stepperWidth = stepperButtonWidth + padding +
                                stepperValueWidth + padding +
                                stepperButtonWidth;
        // Build as stepper
        layout.top.initiativeFrame = left.control(stepperWidth, spec.preferredHeight, padding);
        layout.top.iniativeStepperRects = Stepper::buildStepperRects(
            layout.top.initiativeFrame,
            rowHeight,
            0.6
        );

        qDebug() << "initiativeFrame " << layout.top.initiativeFrame;
    } else {
        // Build as simple text box
        layout.top.initiativeFrame = left.text(spec.initiativeWidth, padding);
        layout.top.iniativeStepperRects = {}; // leave empty
    }

    // faction circle rect (square to keep proportions)
    int factionDiameter = layout.mainRowRect.height() * 0.6; // tweak scale as needed
    layout.top.factionRect = left.control(factionDiameter, factionDiameter, padding);


    // --- right-hand controls (delete, optional icon selector) ---
    LayoutUtils::RightAnchorBuilder right(layout.mainRowRect);
    layout.top.deleteButtonRect = right.icon(spec.deleteButtonScale, padding);

    if (isSelectedIndex && !character->hasActed()) {
        // build rects for all default actions
        const QList<QIcon> icons = IconRepository::actionIcons();
        const int iconCount = icons.size();
        const int iconSize  = static_cast<int>(layout.mainRowRect.height() * spec.iconSelectorIconScale);
        const int totalW    = iconCount * iconSize + (iconCount - 1) * padding;

        layout.top.iconSelectorRect = right.control(totalW, iconSize, padding);
        layout.top.rightAnchorRect = layout.top.iconSelectorRect;
        layout.top.actionIconRects = buildIconRects(layout.top.iconSelectorRect, iconCount, spec.padding);
    } else if (isSelectedIndex && character->hasActed()) {
        // cancel icon as a one-item selector
        const int iconSize = static_cast<int>(layout.mainRowRect.height() * spec.iconSelectorIconScale);
        layout.top.iconSelectorRect = right.control(iconSize, iconSize, padding);
        layout.top.rightAnchorRect  = layout.top.iconSelectorRect;
        layout.top.actionIconRects  = { layout.top.iconSelectorRect }; // single rect
    } else {
        // nothing
        layout.top.iconSelectorRect = QRect();
        layout.top.rightAnchorRect  = layout.top.deleteButtonRect;
        layout.top.actionIconRects  = {}; // empty
    }


    // --- text, fills space between factionRect and rightAnchorRect ---
    int availableW = layout.top.rightAnchorRect.left() - layout.top.factionRect.right();
    layout.top.textRect = left.text(availableW, spec.padding);

    // --- dropdowns (expanded state) ---
    if (isExpanded) {
        if (character->actionType() == Cyrus::ActionType::Attack) {
            layout.attack = buildAttackRects(layout.dropdownRect, spec);
        }
        if (character->actionType() == Cyrus::ActionType::Cast) {
            layout.cast = buildCastRects(layout.dropdownRect, spec);
        }
        if (character->actionType() == Cyrus::ActionType::Misc) {
            layout.misc = buildMiscRects(layout.dropdownRect, spec);
        }
    }

    return std::make_shared<CharacterLayout>(layout);
}



std::optional<HitCommand> CharacterLayoutEngine::hitTest(
    const QModelIndex& index,
    const std::shared_ptr<Layout>& layout,
    const std::shared_ptr<Character>& character,
    const QPoint& cursorPos,
    const Cyrus::CombatSequenceState state
) {
    auto cLayout = std::static_pointer_cast<CharacterLayout>(layout);
    const QUuid id = character->uuid();

    // Hit test for initiative stepper if the state is INITIATIVE
    if (state == Cyrus::CombatSequenceState::INITIATIVE) {
        const auto& stepper = cLayout->top.iniativeStepperRects;

        if (stepper.leftRect.contains(cursorPos)) {
            qDebug() << "Initiative minus clicked." << id << " index " << index.row();
            return HitCommand{ [index, this](QListView* /*view*/) {
                emit decrementInitiativeClicked(index);
            }};
        }
        if (stepper.rightRect.contains(cursorPos)) {
            qDebug() << "Initiative plus clicked." << id << " index " << index.row();
            return HitCommand{ [index, this](QListView* /*view*/) {
                emit incrementInitiativeClicked(index);
            }};
        }
        // TODO implement editing
        /*if (stepper.valueRect.contains(cursorPos)) {
            // Optional: could treat clicking the number as an edit action
            qDebug() << "Initiative value clicked." << id << " index " << index.row();
            return HitCommand{ [index, this](QListView* view) {
                if (auto* initiativeView = qobject_cast<InitiativeView*>(view)) {
                    initiativeView->editCustom(index);
                }
            }};
        }*/
    }

    // Delete button
    if (cLayout->top.deleteButtonRect.contains(cursorPos)) {
        return HitCommand{ [index, this](QListView* /*view*/) {
            qDebug() << "Delete button clicked.";
            emit deleteItemClicked(index);
        }};
    }

    // Action icons
    if (!character->hasActed()) {
        // Normal action icons
        for (int actionIconIndex = 0; actionIconIndex < cLayout->top.actionIconRects.size(); ++actionIconIndex) {
            if (cLayout->top.actionIconRects[actionIconIndex].contains(cursorPos)) {
                return HitCommand{ [index, actionIconIndex, this](QListView* /*view*/) {
                    qDebug() << "Action Icon " << QString::number(actionIconIndex) << " clicked.";
                    emit iconSelectorClicked(index, static_cast<Cyrus::ActionType>(actionIconIndex));
                }};
            }
        }
    } else {
        // Cancel icon
        if (!cLayout->top.actionIconRects.isEmpty() &&
            cLayout->top.actionIconRects[0].contains(cursorPos)) {
            return HitCommand{ [index, this](QListView* /*view*/) {
                qDebug() << "Cancel icon clicked.";
                emit cancelActionClicked(index);   // <-- you define this signal in controller
            }};
        }
    }

    // Cast actions
    if (character->actionType() == Cyrus::ActionType::Cast && cLayout->cast) {
        auto& cast = *cLayout->cast;
        if (cast.spellEdit.contains(cursorPos)) {
            qDebug() << "Cast spell edit clicked." << id << " index " << index.row();
            return HitCommand{ [index, this](QListView* view) { 
                if (auto* initiativeView = qobject_cast<InitiativeView*>(view)){;
                    initiativeView->editCustom(index);
                }
            }};
        }
        if (cast.castingTimeStepperRects.leftRect.contains(cursorPos)) {
            qDebug() << "Cast casting time minus button clicked." << id << " index " << index.row();
            return HitCommand{ [id, this](QListView* /*view*/) { emit decrementCastingTimeClicked(id); } };
        }
        if (cast.castingTimeStepperRects.rightRect.contains(cursorPos)) {
            qDebug() << "Cast casting time plus button clicked." << id << " index " << index.row();
            return HitCommand{ [id, this](QListView* /*view*/) { emit incrementCastingTimeClicked(id); } };
        }
        if (cast.durationStepperRects.leftRect.contains(cursorPos)) {
            qDebug() << "Cast duration minus button clicked." << id << " index " << index.row();
            return HitCommand{ [id, this](QListView* /*view*/) { emit decrementDurationClicked(id); } };
        }
        if (cast.durationStepperRects.rightRect.contains(cursorPos)) {
            qDebug() << "Cast duration plus button clicked." << id << " index " << index.row();
            return HitCommand{ [id, this](QListView* /*view*/) { emit incrementDurationClicked(id); } };
        }
        if (cast.submitButtonRect.contains(cursorPos)) {
            qDebug() << "Cast submit button clicked." << id << " index " << index.row();
            return HitCommand{ [id, index, this, character](QListView* /*view*/) {
                emit castSubmitClicked(id, character);
            }};
        }
    }

    // Attack actions
    if (character->actionType() == Cyrus::ActionType::Attack && cLayout->attack) {
        auto& attack = *cLayout->attack;

        if (attack.attackAmountStepperRects.leftRect.contains(cursorPos)) {
            return HitCommand{ [index, this](QListView* /*view*/) { emit decrementAttackAmountClicked(index); } };
        }
        if (attack.attackAmountStepperRects.rightRect.contains(cursorPos)) {
            return HitCommand{ [index, this](QListView* /*view*/) { emit incrementAttackAmountClicked(index); } };
        }
        if (attack.weaponSpeedStepperRects.leftRect.contains(cursorPos)) {
            return HitCommand{ [index, this](QListView* /*view*/) { emit decrementWeaponSpeedClicked(index); } };
        }
        if (attack.weaponSpeedStepperRects.rightRect.contains(cursorPos)) {
            return HitCommand{ [index, this](QListView* /*view*/) { emit incrementWeaponSpeedClicked(index); } };
        }
        if (attack.submitButtonRect.contains(cursorPos)) {
            qDebug() << "Attack submit button clicked.";
            return HitCommand{ [index, this, character](QListView* /*view*/) {
                emit attackSubmitClicked(index);
            }};
        }
    }

    if (character->actionType() == Cyrus::ActionType::Misc && cLayout->misc) {
        auto& misc = *cLayout->misc;

        for (int miscActionIconIndex = 0; miscActionIconIndex < misc.miscActionIconRects.size(); ++miscActionIconIndex) {
            if (misc.miscActionIconRects[miscActionIconIndex].contains(cursorPos)) {
                return HitCommand{ [index, miscActionIconIndex, this](QListView* /*view*/) {
                    qDebug() << "Misc Action Icon " << QString::number(miscActionIconIndex) << " clicked.";
                    emit iconSelectorClicked(index, static_cast<Cyrus::MiscActionType>(miscActionIconIndex));
                }};
            }
        }

        if (misc.submitButtonRect.contains(cursorPos)) {
            qDebug() << "Misc submit button clicked.";
            return HitCommand{ [index, this, character](QListView* /*view*/) {
                emit miscSubmitClicked(index);
            }};
        }
    }

    return std::nullopt;
}

void CharacterLayoutEngine::paintLayout(
        QPainter* painter,
        const std::shared_ptr<Layout>& layout,
        const std::shared_ptr<Character>& character,
        const CastState castState,
        bool isSelectedIndex,
        bool isExpanded,
        const QPoint& localCursor,
        const Cyrus::CombatSequenceState state
    ) const {

    auto cLayout = std::static_pointer_cast<CharacterLayout>(layout);
    auto spec = character->layoutSpec();

    bool isPressed = QApplication::mouseButtons() & Qt::LeftButton;

    // --- Hero icon ---
    QIcon heroIcon = character->icon();
    heroIcon.paint(painter, cLayout->top.heroIconRect, Qt::AlignCenter,
                   QIcon::Normal, QIcon::On);

    // --- Initiative number / stepper ---

    painter->setFont(StyleRepository::textFont(14, true));
    if (state == Cyrus::CombatSequenceState::INITIATIVE) {
        StepperState hover = Stepper::buildHoverState(cLayout->top.iniativeStepperRects, localCursor);
        Stepper::paintStepper<int>(
            painter,
            cLayout->top.iniativeStepperRects,
            hover,
            character->initiative()
        );
    } 
    else 
    {
        painter->setPen(ColorRepository::selectedForeground());
        painter->drawText(
            cLayout->top.initiativeFrame,
            Qt::AlignVCenter | Qt::AlignCenter,
            QString::number(character->initiative())
        );
    }

    // --- Faction color circle ---
    QColor factionColor = ColorRepository::colorForFaction(character->faction());

    painter->setRenderHint(QPainter::Antialiasing, true);

    // Use the factionRect from layout for positioning
    QRect outerRect = cLayout->top.factionRect;

    // --- Outer gold ring ---
    painter->setPen(QPen(ColorRepository::selectedForeground(), 1.5));
    painter->setBrush(ColorRepository::buttonBackground());
    painter->drawRoundedRect(
        outerRect,
        outerRect.height() / 2,
        outerRect.height() / 2
    );

    painter->setBrush(factionColor);
    painter->setPen(Qt::NoPen);

    QRect innerRect = outerRect.adjusted(3, 3, -3, -3); 
    painter->drawRoundedRect(
        innerRect,
        innerRect.height() / 2,
        innerRect.height() / 2
    );

    // --- Name ---
    painter->setFont(StyleRepository::textFont(14, true));
    painter->setPen(ColorRepository::text());
    painter->drawText(cLayout->top.textRect, Qt::AlignVCenter | Qt::AlignLeft,
                      character->text());

    // --- Delete button ---
    PainterUtils::paintIcon(
        painter,
        IconRepository::delete_icon(),
        cLayout->top.deleteButtonRect,
        false,
        cLayout->top.deleteButtonRect.contains(localCursor),
        cLayout->top.deleteButtonRect.contains(localCursor) && isPressed,
        cLayout->top.deleteButtonRect.height()
    );

    // --- Action icons ---
    bool isBaseCharacter = checkIfCharacter(character);
    if (isSelectedIndex && isBaseCharacter) {

        if (!character->hasActed()) {
            // Paint multi-action selector
            const QList<QIcon> actionIcons = IconRepository::actionIcons();

            for (int i = 0; i < cLayout->top.actionIconRects.size(); ++i) {
                const QRect& iconRect = cLayout->top.actionIconRects[i];
                bool selected    = (static_cast<int>(character->actionType()) == i);
                bool hovered     = iconRect.contains(localCursor);
                bool iconPressed = hovered && isPressed;

                PainterUtils::paintIcon(painter, actionIcons[i], iconRect, selected, hovered, iconPressed, iconRect.height());
            }
        } else {
            // Paint cancel button as single action
            if (!cLayout->top.actionIconRects.isEmpty()) {
                const QRect& iconRect = cLayout->top.actionIconRects.first();
                bool hovered     = iconRect.contains(localCursor);
                bool iconPressed = hovered && isPressed;

                PainterUtils::paintIcon(painter, IconRepository::cancel_icon(), iconRect, /*selected*/ false, hovered, iconPressed, iconRect.height());
            }
        }

    }

    // --- Dropdown painting ---
    if (isExpanded) {
        switch (character->actionType()) {
            case Cyrus::ActionType::Attack:
                if (cLayout->attack) paintAttackDropdown(painter, cLayout->dropdownRect, cLayout->attack.value(), character, localCursor);
                break;
            case Cyrus::ActionType::Cast:
                if (cLayout->cast) paintCastDropdown(painter, cLayout->dropdownRect, cLayout->cast.value(), castState, localCursor);
                break;
            case Cyrus::ActionType::Misc:
                if (cLayout->misc) paintMiscDropdown(painter, cLayout->dropdownRect, cLayout->misc.value(), character, localCursor);
                break;
            default:
                break;
        }
    }
}


int CharacterLayoutEngine::minimumWidth(
        const std::shared_ptr<Character>& character) const {
    // Minimum attack row width not implemented
    // for now it doessnt matter as the casst row is always way wider
    return minimumCastRowWidth(*character);
}


// ----------------- Icon selector + rows -----------------

QVector<QRect> CharacterLayoutEngine::buildIconRects(const QRect& iconSelectorRect, int iconCount, int padding) {

    QVector<QRect> rects;

    if (iconCount <= 0) return rects;

    // Calculate uniform icon width to fit within the frame
    int totalPadding = (iconCount - 1) * padding;
    int iconWidth = (iconSelectorRect.width() - totalPadding) / iconCount;
    int iconHeight = iconSelectorRect.height(); // full height of the selector

    int x = iconSelectorRect.left();
    for (int i = 0; i < iconCount; ++i) {
        rects << QRect(x, iconSelectorRect.top(), iconWidth, iconHeight);
        x += iconWidth + padding;
    }

    return rects;
}


// ----------------- build rects helpers -----------------
AttackRects CharacterLayoutEngine::buildAttackRects(const QRect& dropdownRect,
                                                    const LayoutSpec& spec) {
    AttackRects a;

    //locals
    const int stepperButtonWidth = 22;
    const int stepperValueWidth  = 40;
    const int padding            = spec.padding;

    int rowHeight                = dropdownRect.height();
    const int attackAmountLabelWidth  = CharacterLayoutEngine::attackAmountLabelWidth();
    const int weaponSpeedLabelWidth = CharacterLayoutEngine::weaponSpeedLabelWidth();

    // Stepper width = minus + padding + value + padding + plus
    const int stepperWidth = stepperButtonWidth + padding + stepperValueWidth + padding + stepperButtonWidth;

    LayoutUtils::LeftAnchorBuilder left(dropdownRect);

    a.attackAmountLabel = left.text(attackAmountLabelWidth, padding);

    // attack amount Stepper
    a.attackAmountFrame = left.control(stepperWidth, rowHeight, padding);
    a.attackAmountStepperRects = Stepper::buildStepperRects(
        a.attackAmountFrame,
        rowHeight,
        0.6
    );

    a.weaponSpeedLabel = left.text(weaponSpeedLabelWidth, padding);

    //weapon speed steppers
    a.weaponSpeedFrame = left.control(stepperWidth, rowHeight, padding);
    a.weaponSpeedStepperRects = Stepper::buildStepperRects(
        a.weaponSpeedFrame,
        rowHeight,
        0.6
    );

    // Submit button: anchored to right padding
    LayoutUtils::RightAnchorBuilder right(dropdownRect);
    a.submitButtonRect = right.icon(spec.submitButtonScale, padding);

    return a;
}



CastRects CharacterLayoutEngine::buildCastRects(const QRect& dropdownRect,
                                                const LayoutSpec& spec) {
    CastRects c;

    // Local vars
    const int stepperButtonWidth = 22;
    const int stepperValueWidth  = 40;
    const int padding            = spec.padding;
    int rowHeight                = dropdownRect.height();

    const int castingTimeLabelWidth = CharacterLayoutEngine::castingTimeLabelWidth();
    const int durationLabelWidth    = CharacterLayoutEngine::durationLabelWidth();

    const int stepperWidth = stepperButtonWidth + padding +
                             stepperValueWidth + padding +
                             stepperButtonWidth;

    // Build out controls
    LayoutUtils::LeftAnchorBuilder left(dropdownRect);

    const int submitButtonSize = static_cast<int>(rowHeight * spec.submitButtonScale); // scale-based

    // calculate how much space will be available to the spell edit control
    int reservedWidth =
        (castingTimeLabelWidth + padding + stepperWidth + (padding * 2)) +
        (durationLabelWidth    + padding + stepperWidth + (padding * 2)) +
        (submitButtonSize + (padding * 4));

    int spellEditWidth = std::max(100, dropdownRect.width() - reservedWidth - (padding * 2));

    // Spell edit
    c.spellEdit = left.text(spellEditWidth, padding, dropdownRect.height() * 0.5);

    // Casting time
    c.castingTimeLabel = left.text(castingTimeLabelWidth, padding);
    c.castingTimeFrame = left.control(stepperWidth, rowHeight, padding);
    c.castingTimeStepperRects = Stepper::buildStepperRects(c.castingTimeFrame, rowHeight, 0.6);


    // Duration
    c.durationLabel = left.text(durationLabelWidth, padding);
    c.durationFrame = left.control(stepperWidth, rowHeight, padding);

    c.durationStepperRects = Stepper::buildStepperRects(c.durationFrame, rowHeight, 0.6);

    // Submit button
    LayoutUtils::RightAnchorBuilder right(dropdownRect);
    c.submitButtonRect = right.icon(spec.submitButtonScale, padding);

    return c;
}

MiscRects CharacterLayoutEngine::buildMiscRects(const QRect& dropdownRect,
                                                const LayoutSpec& spec) {
    MiscRects m;

    // --- Local vars ---
    const int padding   = spec.padding;
    int rowHeight       = dropdownRect.height();

    LayoutUtils::LeftAnchorBuilder left(dropdownRect);
    LayoutUtils::RightAnchorBuilder right(dropdownRect);

    const int submitButtonSize = static_cast<int>(rowHeight * spec.submitButtonScale);

    // --- Misc action icons ---
    const QList<QIcon> icons = IconRepository::miscActionIcons();
    const int iconCount = icons.size();
    const int iconSize  = static_cast<int>(rowHeight * spec.iconSelectorIconScale);
    const int totalW    = iconCount * iconSize + (iconCount - 1) * padding;

    m.iconSelectorRect    = left.control(totalW, iconSize, padding);
    m.miscActionIconRects = buildIconRects(m.iconSelectorRect, iconCount, padding);

    // --- Action cost label and value ---
    const int labelWidth = 40;  // Width for "Cost:" label
    const int valueWidth = 40;  // Width for cost value
    m.actionCostLabel = left.text(labelWidth, padding, rowHeight * 0.5);
    m.actionCost      = left.text(valueWidth, padding, rowHeight * 0.5);

    // --- Submit button ---
    m.submitButtonRect = right.icon(spec.submitButtonScale, padding);

    return m;
}


// Calculate the minimal width of the cast row

int CharacterLayoutEngine::minimumCastRowWidth(const Character& character) {
    auto spec = character.layoutSpec();

    const int controlHeight      = 24;
    const int stepperButtonWidth = 22;
    const int stepperValueWidth  = 40;
    const int padding            = spec.padding / 2;

    const int castingTimeLabelW = CharacterLayoutEngine::castingTimeLabelWidth();
    const int durationLabelW    = CharacterLayoutEngine::durationLabelWidth();

    const int stepperWidth =
        stepperButtonWidth + padding +
        stepperValueWidth  + padding +
        stepperButtonWidth;

    // Use scaled submit button size based on preferred row height
    const int submitButtonSize = static_cast<int>(2 * spec.preferredHeight * spec.submitButtonScale);

    const int minSpellEditW  = 100; // minimum spell edit width

    // Build up required width
    int width = 0;

    // spell edit
    width += minSpellEditW + (padding * 2);

    // casting time label + stepper
    width += castingTimeLabelW + padding + stepperWidth + (padding * 2);

    // duration label + stepper
    width += durationLabelW + padding + stepperWidth + (padding * 2);

    // OK / submit button
    width += submitButtonSize + (padding * 4);

    // outer margins
    width += 2 * padding;

    return width;
}

int CharacterLayoutEngine::castingTimeLabelWidth(){
    QFont f = StyleRepository::textFont(14, false);
    QFontMetrics fm(f);
    return fm.horizontalAdvance(QStringLiteral("Spd:")) + 2;
}

int CharacterLayoutEngine::durationLabelWidth(){
    QFont f = StyleRepository::textFont(14, false);
    QFontMetrics fm(f);
    return fm.horizontalAdvance(QStringLiteral("Dur:")) + 2;
}


int CharacterLayoutEngine::attackAmountLabelWidth(){
    QFont f = StyleRepository::textFont(14, false);
    QFontMetrics fm(f);
    return fm.horizontalAdvance(QStringLiteral("Rate:")) + 2;
}

int CharacterLayoutEngine::weaponSpeedLabelWidth(){
    QFont f = StyleRepository::textFont(14, false);
    QFontMetrics fm(f);
    return fm.horizontalAdvance(QStringLiteral("WS:")) + 2;
}

bool CharacterLayoutEngine::checkIfCharacter(const std::shared_ptr<Character>& character) const {
    return typeid(*character) == typeid(Character);
}

void CharacterLayoutEngine::paintAttackDropdown(QPainter* painter,
                                                const QRect& dropdownRect,
                                                const AttackRects& attack,
                                                const std::shared_ptr<Character>& character,
                                                const QPoint& localCursor) const{

    // Locals 
    bool isPressed = QApplication::mouseButtons() & Qt::LeftButton;

    // --- Labels ---
    painter->setFont(StyleRepository::textFont(10, false));
    painter->setPen(ColorRepository::text());
    painter->drawText(attack.attackAmountLabel,     Qt::AlignVCenter | Qt::AlignLeft, "Rate:");
    painter->drawText(attack.weaponSpeedLabel,    Qt::AlignVCenter | Qt::AlignLeft, "WS:");

    // steppers
    StepperState attackAmountHoverState = Stepper::buildHoverState(attack.attackAmountStepperRects, localCursor);
    painter->setFont(StyleRepository::textFont(14, true));
    Stepper::paintStepper(
        painter,
        attack.attackAmountStepperRects,
        attackAmountHoverState,
        character->attackRate()
    );

    StepperState weaponSpeedHoverState = Stepper::buildHoverState(attack.weaponSpeedStepperRects, localCursor);
    Stepper::paintStepper(
        painter,
        attack.weaponSpeedStepperRects,
        weaponSpeedHoverState,
        character->weaponSpeed()
    );

    // Draw Submit button
    PainterUtils::paintIcon(
        painter,
        IconRepository::submit_icon(),
        attack.submitButtonRect,
        false,
        attack.submitButtonRect.contains(localCursor),
        attack.submitButtonRect.contains(localCursor) && isPressed,
        attack.submitButtonRect.height()
    );
}


void CharacterLayoutEngine::paintCastDropdown(QPainter* painter,
                                              const QRect& dropdownRect,
                                              const CastRects& cast,
                                              const CastState& castState,
                                              const QPoint& localCursor) const 
{
    // Locals 
    bool isPressed = QApplication::mouseButtons() & Qt::LeftButton;

    painter->save();

    // --- Labels ---
    painter->setFont(StyleRepository::textFont(10, false));
    painter->setPen(ColorRepository::text());
    painter->drawText(cast.castingTimeLabel, Qt::AlignVCenter | Qt::AlignLeft, "Spd:");
    painter->drawText(cast.durationLabel,    Qt::AlignVCenter | Qt::AlignLeft, "Dur:");

    // --- Spell edit ---
    painter->setPen(QPen(ColorRepository::buttonOutlineColor(), 1.0));
    painter->setBrush(ColorRepository::baseBackground());
    painter->drawRoundedRect(cast.spellEdit, 6, 6);

    painter->setPen(castState.isDefaulted()
                        ? ColorRepository::placeholderText()
                        : ColorRepository::text());
    const QString spellText = castState.isDefaulted()
                                ? castState.defaultText
                                : castState.spellName;
    painter->drawText(cast.spellEdit.adjusted(11, 2, -6, 0),
                      Qt::AlignVCenter | Qt::AlignLeft, spellText);


    StepperState castingTimeHoverState = Stepper::buildHoverState(cast.castingTimeStepperRects, localCursor);

    painter->setFont(StyleRepository::textFont(14, true));

    Stepper::paintStepper(
        painter,
        cast.castingTimeStepperRects,
        castingTimeHoverState,
        castState.castingTime
    );

    StepperState durationHoverState = Stepper::buildHoverState(cast.durationStepperRects, localCursor);

    Stepper::paintStepper( // CHANGED
        painter,
        cast.durationStepperRects,
        durationHoverState,
        castState.duration
    );
    
    // --- Submit button ---
    PainterUtils::paintIcon(
        painter,
        IconRepository::submit_icon(),
        cast.submitButtonRect,
        false,
        cast.submitButtonRect.contains(localCursor),
        cast.submitButtonRect.contains(localCursor) && isPressed,
        cast.submitButtonRect.height()
    );

    painter->restore();
}

void CharacterLayoutEngine::paintMiscDropdown(QPainter* painter,
                            const QRect& dropdownRect,
                            const MiscRects& misc,
                            const std::shared_ptr<Character>& character,
                            const QPoint& localCursor) const {
    bool isPressed = QApplication::mouseButtons() & Qt::LeftButton;

    painter->save();

    auto spec = character->layoutSpec();

    // --- Icon selector ---
    const QList<QIcon> miscIcons = IconRepository::miscActionIcons();
    for (int i = 0; i < misc.miscActionIconRects.size(); ++i) {
        const QRect& iconRect = misc.miscActionIconRects[i];
        bool selected    = (static_cast<int>(character->miscActionType()) == i);
        bool hovered     = iconRect.contains(localCursor);
        bool iconPressed = hovered && isPressed;

        PainterUtils::paintIcon(
            painter,
            miscIcons[i],
            iconRect,
            selected,
            hovered,
            iconPressed,
            iconRect.height()
        );
    }

    // --- Action cost label ---
    painter->setFont(StyleRepository::textFont(12, true));
    painter->setPen(ColorRepository::text());
    painter->drawText(misc.actionCostLabel,
                      Qt::AlignVCenter | Qt::AlignLeft,
                      QString("Cost:"));

    // --- Action cost value in a colored circle ---
    int diameter = qMin(misc.actionCost.width(), misc.actionCost.height());
    QRect circleRect(
        misc.actionCost.center().x() - diameter / 2,
        misc.actionCost.center().y() - diameter / 2,
        diameter,
        diameter
    );

    painter->setBrush(ColorRepository::buttonBackground());     
    painter->setPen(ColorRepository::buttonOutlineColor());   
    painter->drawEllipse(circleRect); 

    painter->setPen(ColorRepository::text());             
    painter->drawText(circleRect,
                    Qt::AlignCenter,
                    QString::number(character->miscActionCost()));


    // --- Submit button ---
    bool submitHovered = misc.submitButtonRect.contains(localCursor);
    bool submitPressed = submitHovered && isPressed;

    PainterUtils::paintIcon(
        painter,
        IconRepository::submit_icon(),
        misc.submitButtonRect,
        false,
        submitHovered,
        submitPressed,
        misc.submitButtonRect.height()
    );

    painter->restore();
}

