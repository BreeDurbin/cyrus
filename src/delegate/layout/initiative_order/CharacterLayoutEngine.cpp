#include "CharacterLayoutEngine.h"

#include "IconRepository.h"
#include "StyleRepository.h"
#include "LayoutUtils.h"

#include <algorithm>
#include "IconRepository.h"
#include "ColorRepository.h"
#include "StyleRepository.h"
#include <QPainter>
#include "PainterUtils.h"
#include "InitiativeView.h"
#include <QApplication>

// ----------------- Public API -----------------

std::shared_ptr<Layout> CharacterLayoutEngine::calculate(
    const QRect& rect,
    const std::shared_ptr<Character>& character,
    bool isActiveIndex,
    bool isExpanded) const 
{
    CharacterLayout layout;
    auto spec = character->layoutSpec();
    int padding = spec.padding;

    // --- base + rows ---
    layout.baseRect     = LayoutUtils::padRectangle(rect, padding); //unpadded, used to build the top and bottom row rects
    layout.mainRowRect  = buildMainRowRect(rect, isExpanded, padding);
    layout.dropdownRect = isExpanded ? buildDropdownRect(rect, layout.mainRowRect, padding) : QRect();

    qDebug() << "Main row rect collapsed: " << buildMainRowRect(rect, false, padding);
    qDebug() << "Main row rect expanded: " << buildMainRowRect(rect, true, padding);

    // --- left-hand controls (hero + initiative) ---
    LayoutUtils::LeftAnchorBuilder left(layout.mainRowRect);
    layout.top.heroIconRect   = left.icon(spec.heroIconScale, padding);
    layout.top.initiativeRect = left.text(spec.initiativeWidth, spec.padding);


    // --- right-hand controls (delete, optional icon selector) ---
    LayoutUtils::RightAnchorBuilder right(layout.mainRowRect);
    layout.top.deleteButtonRect = right.icon(spec.deleteButtonScale, padding);

    if (isActiveIndex) {
        const QList<QIcon> icons = IconRepository::actionIcons();
        const int iconCount = icons.size();
        const int iconSize  = static_cast<int>(layout.mainRowRect.height() * spec.iconSelectorIconScale);
        const int totalW    = iconCount * iconSize + (iconCount - 1) * padding;

        layout.top.iconSelectorRect = right.control(totalW, iconSize, padding);
        layout.top.rightAnchorRect = layout.top.iconSelectorRect;
        layout.top.actionIconRects = buildActionIconRects(layout.top.iconSelectorRect, layout.mainRowRect.height(), spec);
    } else {
        layout.top.iconSelectorRect = QRect();
        layout.top.rightAnchorRect  = layout.top.deleteButtonRect;
    }

    // --- text ---
    layout.top.textRect = left.text(layout.top.rightAnchorRect.left() - layout.top.initiativeRect.right(), spec.padding);

    // --- dropdowns (expanded state) ---
    if (isExpanded) {
        if (character->actionType() == Cyrus::ActionType::Attack) {
            layout.attack = buildAttackRects(layout.dropdownRect, spec);
        }
        if (character->actionType() == Cyrus::ActionType::Cast) {
            layout.cast = buildCastRects(layout.dropdownRect, spec);
        }
    }

    return std::make_shared<CharacterLayout>(layout);
}



std::optional<HitCommand> CharacterLayoutEngine::hitTest(
    const QModelIndex& index,
    const std::shared_ptr<Layout>& layout,
    const std::shared_ptr<Character>& character,
    const QPoint& cursorPos) {
    auto cLayout = std::static_pointer_cast<CharacterLayout>(layout);
    const QUuid id = character->uuid();

    // Delete button
    if (cLayout->top.deleteButtonRect.contains(cursorPos)) {
        return HitCommand{ [index, this](QListView* /*view*/) {
            qDebug() << "Delete button clicked.";
            emit deleteItemClicked(index);
        }};
    }

    // Action icons
    for (int actionIconIndex = 0; actionIconIndex < cLayout->top.actionIconRects.size(); ++actionIconIndex) {
        if (cLayout->top.actionIconRects[actionIconIndex].contains(cursorPos)) {
            return HitCommand{ [index, actionIconIndex, this](QListView* /*view*/) {
                qDebug() << "Action Icon " << QString::number(actionIconIndex) << " clicked.";
                emit iconSelectorClicked(index, static_cast<Cyrus::ActionType>(actionIconIndex));
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
        if (cast.castingTimeStepperRects.minusRect.contains(cursorPos)) {
            qDebug() << "Cast casting time minus button clicked." << id << " index " << index.row();
            return HitCommand{ [id, this](QListView* /*view*/) { emit decrementCastingTimeClicked(id); } };
        }
        if (cast.castingTimeStepperRects.plusRect.contains(cursorPos)) {
            qDebug() << "Cast casting time plus button clicked." << id << " index " << index.row();
            return HitCommand{ [id, this](QListView* /*view*/) { emit incrementCastingTimeClicked(id); } };
        }
        if (cast.durationStepperRects.minusRect.contains(cursorPos)) {
            qDebug() << "Cast duration minus button clicked." << id << " index " << index.row();
            return HitCommand{ [id, this](QListView* /*view*/) { emit decrementDurationClicked(id); } };
        }
        if (cast.durationStepperRects.plusRect.contains(cursorPos)) {
            qDebug() << "Cast duration plus button clicked." << id << " index " << index.row();
            return HitCommand{ [id, this](QListView* /*view*/) { emit incrementDurationClicked(id); } };
        }
        if (cast.submitButtonRect.contains(cursorPos)) {
            qDebug() << "Cast submit button clicked." << id << " index " << index.row();
            return HitCommand{ [id, index, this, character](QListView* /*view*/) {
                emit castSubmitClicked(id, character);
                //emit iconSelectorClicked(index, Cyrus::ActionType::None);
            }};
        }
    }

    // Attack actions
    if (character->actionType() == Cyrus::ActionType::Attack && cLayout->attack) {
        auto& attack = *cLayout->attack;

        if (attack.attackAmountStepperRects.minusRect.contains(cursorPos)) {
            return HitCommand{ [index, this](QListView* /*view*/) { emit decrementAttackAmountClicked(index); } };
        }
        if (attack.attackAmountStepperRects.plusRect.contains(cursorPos)) {
            return HitCommand{ [index, this](QListView* /*view*/) { emit incrementAttackAmountClicked(index); } };
        }
        if (attack.submitButtonRect.contains(cursorPos)) {
            qDebug() << "Attack submit button clicked.";
            return HitCommand{ [index, this, character](QListView* /*view*/) {
                emit attackSubmitClicked(index);
                emit iconSelectorClicked(index, Cyrus::ActionType::None);
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
        bool isActiveIndex,
        bool isExpanded,
        const QPoint& localCursor) const {

    auto cLayout = std::static_pointer_cast<CharacterLayout>(layout);
    auto spec = character->layoutSpec();

    bool isPressed = QApplication::mouseButtons() & Qt::LeftButton;

    // --- Hero icon ---
    QIcon heroIcon = character->icon();
    heroIcon.paint(painter, cLayout->top.heroIconRect, Qt::AlignCenter,
                   QIcon::Normal, QIcon::On);

    // --- Initiative number ---
    painter->setFont(StyleRepository::labelFont(14, true));
    painter->setPen(ColorRepository::selectedForeground());
    painter->drawText(cLayout->top.initiativeRect, Qt::AlignVCenter | Qt::AlignLeft,
                      QString::number(character->initiative()));

    // --- Name ---
    painter->setFont(StyleRepository::labelFont(14, true));
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
    if (isActiveIndex && isBaseCharacter) {
        const QList<QIcon> actionIcons = IconRepository::actionIcons();

        for (int i = 0; i < cLayout->top.actionIconRects.size(); ++i) {
            const QRect& iconRect = cLayout->top.actionIconRects[i];
            bool selected = (static_cast<int>(character->actionType()) == i);
            bool hovered  = iconRect.contains(localCursor);
            bool iconPressed = hovered && isPressed;
            PainterUtils::paintIcon(painter, actionIcons[i], iconRect, selected, hovered, iconPressed, iconRect.height());
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

//---------- Helpers -----------------

QRect CharacterLayoutEngine::buildMainRowRect(const QRect& base, bool isExpanded, int padding) {
    QRect rowRect;

    if (isExpanded) {
        // Take the top half of the base rect
        rowRect = QRect(
            base.left(),
            base.top(),
            base.width(),
            base.height() / 2
        );
    } else {
        // Use the full height when collapsed
        rowRect = base;
    }

    // Apply horizontal and vertical padding
    rowRect.adjust(2 * padding, padding, -(2 * padding), -padding);

    return rowRect;
}

QRect CharacterLayoutEngine::buildDropdownRect(const QRect& base, const QRect& mainRow, int padding) {
    // Remaining space below mainRow
    if (mainRow.bottom() >= base.bottom()) return QRect(); // no space for dropdown

    QRect dropdown(
        base.left(),
        mainRow.bottom(),
        base.width(),
        base.bottom() - mainRow.bottom()
    );

    // Apply horizontal and vertical padding
    dropdown.adjust(2 * padding, padding, -(2 * padding), -padding);

    return dropdown;
}

// ----------------- Icon selector + rows -----------------

QVector<QRect> CharacterLayoutEngine::buildActionIconRects(const QRect& iconSelectorRect,
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



// ----------------- Attack -----------------
AttackRects CharacterLayoutEngine::buildAttackRects(const QRect& dropdownRect,
                                                    const Character::LayoutSpec& spec) {
    AttackRects a;

    const int stepperButtonWidth = 22;
    const int stepperValueWidth  = 40;
    const int controlHeight      = 24;
    const int padding            = spec.padding;

    int rowHeight = dropdownRect.height();

    // Stepper width = minus + padding + value + padding + plus
    const int stepperWidth = stepperButtonWidth + padding + stepperValueWidth + padding + stepperButtonWidth;

    LayoutUtils::LeftAnchorBuilder left(dropdownRect);

    // Stepper frame
    a.attackAmountFrame = left.control(stepperWidth, rowHeight, padding);
    a.attackAmountStepperRects = Stepper::buildStepperRects(
        a.attackAmountFrame,
        stepperButtonWidth,
        stepperValueWidth,
        controlHeight,
        padding
    );

    // Submit button: anchored to right padding
    LayoutUtils::RightAnchorBuilder right(dropdownRect);
    a.submitButtonRect = right.icon(spec.submitButtonScale, padding);

    return a;
}



CastRects CharacterLayoutEngine::buildCastRects(const QRect& dropdownRect,
                                                const Character::LayoutSpec& spec) {
    CastRects c;

    // Local vars
    const int controlHeight      = 24;
    const int stepperButtonWidth = 22;
    const int stepperValueWidth  = 40;
    const int padding            = spec.padding;
    int rowHeight = dropdownRect.height();

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
    c.castingTimeStepperRects = Stepper::buildStepperRects(c.castingTimeFrame,
                                                           stepperButtonWidth,
                                                           stepperValueWidth,
                                                           controlHeight,
                                                           padding);

    // Duration
    c.durationLabel = left.text(durationLabelWidth, padding);
    c.durationFrame = left.control(stepperWidth, rowHeight, padding);
    c.durationStepperRects = Stepper::buildStepperRects(c.durationFrame,
                                                        stepperButtonWidth,
                                                        stepperValueWidth,
                                                        controlHeight,
                                                        padding);

    // Submit button
    LayoutUtils::RightAnchorBuilder right(dropdownRect);
    c.submitButtonRect = right.icon(spec.submitButtonScale, padding);

    return c;
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
    QFont f = StyleRepository::labelFont(14, false);
    QFontMetrics fm(f);
    return fm.horizontalAdvance(QStringLiteral("Speed")) + 6;
}

int CharacterLayoutEngine::durationLabelWidth(){
    QFont f = StyleRepository::labelFont(14, false);
    QFontMetrics fm(f);
    return fm.horizontalAdvance(QStringLiteral("Dur")) + 6;
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

    //qDebug() << "Painting attack dropdown rect " << dropdownRect;

    Stepper::paintStepper(
        painter,
        attack.attackAmountStepperRects,
        character->attackRate()
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
    painter->setFont(StyleRepository::labelFont(10, false));
    painter->setPen(ColorRepository::text());
    painter->drawText(cast.castingTimeLabel, Qt::AlignVCenter | Qt::AlignLeft, "Speed");
    painter->drawText(cast.durationLabel,    Qt::AlignVCenter | Qt::AlignLeft, "Dur");

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

    // --- Steppers ---
    Stepper::paintStepper(painter, cast.castingTimeStepperRects, castState.castingTime);
    Stepper::paintStepper(painter, cast.durationStepperRects,    castState.duration);

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

