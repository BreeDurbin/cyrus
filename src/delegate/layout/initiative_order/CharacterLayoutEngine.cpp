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

// ----------------- Public API -----------------

std::shared_ptr<Layout> CharacterLayoutEngine::calculate(
                            const QRect& rect,
                            const std::shared_ptr<Character>& character,
                            bool isActiveIndex,
                            bool isExpanded) const {
                                
    CharacterLayout layout;
    auto spec = character->layoutSpec();

    // base + rows
    layout.baseRect     = LayoutUtils::padRectangle(rect, spec.padding);
    layout.mainRowRect  = buildMainRowRect(layout.baseRect, isExpanded);
    layout.dropdownRect = isExpanded ? buildDropdownRect(layout.baseRect) : QRect();

    // icon + controls
    layout.top.heroIconRect         = LayoutUtils::buildHeroIconRect(layout.mainRowRect, spec.heroIconScale);
    layout.top.initiativeRect       = LayoutUtils::buildInitiativeRect(layout.top.heroIconRect, spec.padding, spec.initiativeWidth);
    layout.top.deleteButtonRect     = LayoutUtils::buildDeleteRect(layout.mainRowRect, spec.padding, spec.deleteButtonScale);

    // right-hand controls
    layout.top.iconSelectorRect = isActiveIndex
        ? buildIconSelectorRect(layout.top.deleteButtonRect, layout.mainRowRect.height(), spec)
        : QRect();
    layout.top.rightAnchorRect = isActiveIndex ? layout.top.iconSelectorRect : layout.top.deleteButtonRect;

    // text
    layout.top.textRect = LayoutUtils::buildTextRect(layout.top.initiativeRect, layout.top.rightAnchorRect, spec.padding);

    // action icons
    if (isActiveIndex) {
        layout.top.actionIconRects = buildActionIconRects(layout.top.iconSelectorRect, layout.mainRowRect.height(), spec);
    }

    if(isExpanded){
        // attack dropdown
        if (character->actionType() == Cyrus::ActionType::Attack) layout.attack = buildAttackRects(layout.dropdownRect, spec);
        // cast dropdown
        if (character->actionType() == Cyrus::ActionType::Cast) layout.cast = buildCastRects(layout.dropdownRect, spec);
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
        return HitCommand{ [index, this]() {
            emit deleteItemClicked(index);
        }};
    }

    // Action icons
    for (int actionIconIndex = 0; actionIconIndex < cLayout->top.actionIconRects.size(); ++actionIconIndex) {
        if (cLayout->top.actionIconRects[actionIconIndex].contains(cursorPos)) {
            return HitCommand{ [index, actionIconIndex, this]() {
                qDebug() << "Action Icon " << QString::number(actionIconIndex) << " clicked.";
                emit iconSelectorClicked(index, static_cast<Cyrus::ActionType>(actionIconIndex));
            }};
        }
    }

    // Cast actions
    if (character->actionType() == Cyrus::ActionType::Cast && cLayout->cast) {
        auto& cast = *cLayout->cast;

        if (cast.castingTimeStepperRects.minusRect.contains(cursorPos)) {
            return HitCommand{ [id, this]() { emit decrementCastingTimeClicked(id); } };
        }
        if (cast.castingTimeStepperRects.plusRect.contains(cursorPos)) {
            return HitCommand{ [id, this]() { emit incrementCastingTimeClicked(id); } };
        }
        if (cast.durationStepperRects.minusRect.contains(cursorPos)) {
            return HitCommand{ [id, this]() { emit decrementDurationClicked(id); } };
        }
        if (cast.durationStepperRects.plusRect.contains(cursorPos)) {
            return HitCommand{ [id, this]() { emit incrementDurationClicked(id); } };
        }
        if (cast.submitButtonRect.contains(cursorPos)) {
            return HitCommand{ [id, index, this, character]() {
                emit castSubmitted(id, *character);
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
        const CastState& castState,
        bool isActiveIndex,
        bool isExpanded,
        const QPoint& localCursor) const {

    auto cLayout = std::static_pointer_cast<CharacterLayout>(layout);
    auto spec = character->layoutSpec();

    //Debugging for rect size
    qDebug() << "Dropdown rect:" << cLayout->dropdownRect;
    if (cLayout->cast)
        qDebug() << "Cast rects:" << cLayout->cast->castingTimeStepperRects.minusRect
                << cLayout->cast->castingTimeStepperRects.valueRect
                << cLayout->cast->castingTimeStepperRects.plusRect
                << cLayout->cast->submitButtonRect;

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
    PainterUtils::paintDeleteButton(painter,
                                    cLayout->top.deleteButtonRect,
                                    cLayout->top.deleteButtonRect.contains(localCursor));

    // --- Action icons ---
    bool isBaseCharacter = checkIfCharacter(character);
    if (isActiveIndex && isBaseCharacter) {
        const QList<QIcon> actionIcons = IconRepository::actionIcons();
        const int iconSize = static_cast<int>(cLayout->mainRowRect.height() * spec.iconSelectorIconScale);

        for (int i = 0; i < cLayout->top.actionIconRects.size(); ++i) {
            const QRect& iconRect = cLayout->top.actionIconRects[i];
            bool selected = (static_cast<int>(character->actionType()) == i);
            bool hovered  = iconRect.contains(localCursor);

            PainterUtils::paintActionIcon(painter, actionIcons[i], iconRect, selected, hovered, iconSize);
        }
    }

    // --- Dropdown painting ---
    if (isExpanded) {
        switch (character->actionType()) {
            case Cyrus::ActionType::Attack:
                if (cLayout->attack) paintAttackDropdown(painter, cLayout->dropdownRect, cLayout->attack.value(), spec);
                break;
            case Cyrus::ActionType::Cast:
                if (cLayout->cast) paintCastDropdown(painter, cLayout->dropdownRect, cLayout->cast.value(), castState);
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
QRect CharacterLayoutEngine::buildIconSelectorRect(const QRect& deleteRect, int rowHeight,
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

QRect CharacterLayoutEngine::buildMainRowRect(const QRect& base, bool isExpanded) {
    return isExpanded ? base.adjusted(0, 0, 0, -base.height() / 2) : base;
}

//to do work out spacing between itemss sin teh top row and bottom row of expanded rect
QRect CharacterLayoutEngine::buildDropdownRect(const QRect& baseRect) {
    return baseRect.adjusted(0, baseRect.height() / 2, 0, 0);
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
    const int submitButtonSize = static_cast<int>(rowHeight * spec.submitButtonScale); // scale-based

    // Left and right padding inside dropdown
    int leftX  = dropdownRect.left() + padding;
    int rightX = dropdownRect.right() - padding;

    // Stepper width = minus + padding + value + padding + plus
    const int stepperWidth = stepperButtonWidth + padding + stepperValueWidth + padding + stepperButtonWidth;

    // Stepper frame
    a.attackAmountStepperRects = Stepper::buildStepperRects(
        QRect(leftX, dropdownRect.top(), stepperWidth, rowHeight),
        stepperButtonWidth,
        stepperValueWidth,
        controlHeight,
        padding
    );

    // OK button: anchored to right padding
    a.submitButtonRect = QRect(rightX - submitButtonSize,
                     dropdownRect.top() + (rowHeight - submitButtonSize) / 2,
                     submitButtonSize, submitButtonSize);

    return a;
}



CastRects CharacterLayoutEngine::buildCastRects(const QRect& dropdownRect,
                                                const Character::LayoutSpec& spec) {
    CastRects c;

    const int controlHeight      = 24;
    const int stepperButtonWidth = 22;
    const int stepperValueWidth  = 40;
    const int padding            = spec.padding;

    int xPos = padding * 2;        // relative to dropdown
    int yPos = 0;                  // relative to dropdown top
    int rowHeight = dropdownRect.height();

    const int castingTimeLabelWidth = CharacterLayoutEngine::castingTimeLabelWidth();
    const int durationLabelWidth    = CharacterLayoutEngine::durationLabelWidth();

    const int stepperWidth = stepperButtonWidth + padding +
                             stepperValueWidth + padding +
                             stepperButtonWidth;

    const int submitButtonSize = static_cast<int>(rowHeight * spec.submitButtonScale); // scale-based


    int reservedWidth =
        (castingTimeLabelWidth + padding + stepperWidth + (padding * 2)) +
        (durationLabelWidth    + padding + stepperWidth + (padding * 2)) +
        (submitButtonSize + (padding * 4));

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
    c.castingTimeStepperRects = Stepper::buildStepperRects(c.castingTimeFrame,
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
    c.durationStepperRects = Stepper::buildStepperRects(c.durationFrame,
                                                        stepperButtonWidth,
                                                        stepperValueWidth,
                                                        controlHeight,
                                                        padding);
    xPos += stepperWidth + (padding * 2);

    // OK button
    c.submitButtonRect = QRect(xPos,
                     yPos + (rowHeight -submitButtonSize) / 2,
                     submitButtonSize, submitButtonSize);

    return c;
}



//---------- Helpers -----------------

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
                                                const Character::LayoutSpec& spec) const{
    qDebug() << "Painting attack dropdown rect " << dropdownRect;
    // Compute vertical centering
    const int h = dropdownRect.height();
    const int y = dropdownRect.top() + (h - attack.attackAmountStepperRects.valueRect.height()) / 2;

    // Shift stepper rects vertically to match dropdownRect
    StepperRects stepper = attack.attackAmountStepperRects;
    stepper.minusRect.moveTop(y);
    stepper.valueRect.moveTop(y);
    stepper.plusRect.moveTop(y);

    PainterUtils::paintStepper(
        painter,
        attack.attackAmountStepperRects,
        1
    );

    // Draw Submit button
    QRect submitButtonRect = attack.submitButtonRect;
    submitButtonRect.moveTop(y);

    QIcon submitButtonIcon = IconRepository::submit();
    submitButtonIcon.paint(painter, submitButtonRect);
}


void CharacterLayoutEngine::paintCastDropdown(QPainter* painter,
                                            const QRect& dropdownRect,
                                            const CastRects& cast,
                                            const CastState& castState) const {

    // --- Debug: print all cast rects ---
    qDebug() << "Painting cast dropdown. Dropdown rect:" << dropdownRect;
    qDebug() << "  spellEdit:" << cast.spellEdit;
    qDebug() << "  castingTimeLabel:" << cast.castingTimeLabel;
    qDebug() << "  castingTimeFrame:" << cast.castingTimeFrame;
    qDebug() << "  castingTimeStepper.minusRect:" << cast.castingTimeStepperRects.minusRect;
    qDebug() << "  castingTimeStepper.valueRect:" << cast.castingTimeStepperRects.valueRect;
    qDebug() << "  castingTimeStepper.plusRect:" << cast.castingTimeStepperRects.plusRect;
    qDebug() << "  durationLabel:" << cast.durationLabel;
    qDebug() << "  durationFrame:" << cast.durationFrame;
    qDebug() << "  durationStepper.minusRect:" << cast.durationStepperRects.minusRect;
    qDebug() << "  durationStepper.valueRect:" << cast.durationStepperRects.valueRect;
    qDebug() << "  durationStepper.plusRect:" << cast.durationStepperRects.plusRect;
    qDebug() << "  submitButtonRect:" << cast.submitButtonRect;

    painter->setFont(StyleRepository::labelFont(10, false));

    // Compute vertical offset for alignment
    const int h = dropdownRect.height();
    const int yOffset = dropdownRect.top();

    // Labels
    QRect castingTimeLabel = cast.castingTimeLabel.translated(0, yOffset);
    QRect durationLabel    = cast.durationLabel.translated(0, yOffset);

    painter->setPen(ColorRepository::text());
    painter->drawText(castingTimeLabel, Qt::AlignVCenter | Qt::AlignLeft, "Speed");
    painter->drawText(durationLabel,    Qt::AlignVCenter | Qt::AlignLeft, "Dur");

    // Spell edit box
    QRect spellEdit = cast.spellEdit.translated(0, yOffset);
    painter->setPen(QPen(ColorRepository::buttonOutlineColor(), 1.0));
    painter->setBrush(ColorRepository::baseBackground());
    painter->drawRoundedRect(spellEdit, 6, 6);

    painter->setPen(castState.isDefaulted() ? ColorRepository::placeholderText() : ColorRepository::text());
    const QString spellText = castState.isDefaulted() ? castState.defaultText : castState.spellName;
    painter->drawText(spellEdit.adjusted(11, 2, -6, 0),
                      Qt::AlignVCenter | Qt::AlignLeft, spellText);

    // Steppers
    StepperRects castingTimeStepper = cast.castingTimeStepperRects;
    StepperRects durationStepper    = cast.durationStepperRects;
    castingTimeStepper.minusRect.moveTop(castingTimeStepper.minusRect.top() + yOffset);
    castingTimeStepper.valueRect.moveTop(castingTimeStepper.valueRect.top() + yOffset);
    castingTimeStepper.plusRect.moveTop(castingTimeStepper.plusRect.top() + yOffset);
    durationStepper.minusRect.moveTop(durationStepper.minusRect.top() + yOffset);
    durationStepper.valueRect.moveTop(durationStepper.valueRect.top() + yOffset);
    durationStepper.plusRect.moveTop(durationStepper.plusRect.top() + yOffset);

    PainterUtils::paintStepper(painter, castingTimeStepper, castState.castingTime);
    PainterUtils::paintStepper(painter, durationStepper, castState.duration);

    // Draw Submit button
    QRect submitButtonRect = cast.submitButtonRect.translated(0, yOffset);
    QIcon submitButtonIcon = IconRepository::submit();
    submitButtonIcon.paint(painter, submitButtonRect);
}
