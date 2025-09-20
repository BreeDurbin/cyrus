#include "InitiativeDelegate.h"

#include <QStyledItemDelegate>
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QCursor>
#include "ColorRepository.h"
#include "StyleRepository.h"
#include "IconRepository.h"
#include "CharacterModel.h"
#include "Character.h"
#include <QAbstractItemView>
#include "Enums.h"
#include "InitiativeView.h"
#include "PainterUtils.h"
#include "LayoutFactory.h"
#include "LayoutEngine.h"
#include <QLineEdit>
#include <QPainterPath>
#include "CharacterLayoutEngine.h"
#include "AttackActionLayoutEngine.h"
#include "CastActionLayoutEngine.h"
#include "MiscActionLayoutEngine.h"
#include "MiscAction.h"

#include<iostream>

InitiativeDelegate::InitiativeDelegate(CharacterController* characterController, QObject *parent) 
    : QStyledItemDelegate(parent), characterController_{characterController} {

    // character layout engine
    auto* characterLayoutEngine = static_cast<CharacterLayoutEngine*>(LayoutFactory::forCharacterType<Character>());

    // main row
    connect(characterLayoutEngine, &CharacterLayoutEngine::deleteItemClicked,
            this, &InitiativeDelegate::deleteItem);

    connect(characterLayoutEngine, QOverload<const QModelIndex&, Cyrus::ActionType>::of(&CharacterLayoutEngine::iconSelectorClicked),
            this,                  QOverload<const QModelIndex&, Cyrus::ActionType>::of(&InitiativeDelegate::iconSelected));

    connect(characterLayoutEngine, &CharacterLayoutEngine::cancelActionClicked,
        this, &InitiativeDelegate::cancelAction);

    connect(characterLayoutEngine, &CharacterLayoutEngine::decrementInitiativeClicked,
            this, &InitiativeDelegate::decrementInitiative);

    connect(characterLayoutEngine, &CharacterLayoutEngine::incrementInitiativeClicked,
            this, &InitiativeDelegate::incrementInitiative);

    //Cast action dropdown
    connect(characterLayoutEngine, &CharacterLayoutEngine::spellNameEdited,
            this, &InitiativeDelegate::spellNameEdited);

    connect(characterLayoutEngine, &CharacterLayoutEngine::decrementCastingTimeClicked,
            this, &InitiativeDelegate::decrementCastingTime);

    connect(characterLayoutEngine, &CharacterLayoutEngine::incrementCastingTimeClicked,
            this, &InitiativeDelegate::incrementCastingTime);

    connect(characterLayoutEngine, &CharacterLayoutEngine::decrementDurationClicked,
            this, &InitiativeDelegate::decrementDuration);

    connect(characterLayoutEngine, &CharacterLayoutEngine::incrementDurationClicked,
            this, &InitiativeDelegate::incrementDuration);

    connect(characterLayoutEngine, &CharacterLayoutEngine::castSubmitClicked,
            this, &InitiativeDelegate::castSubmitted);

    // Attack action dropdown
    connect(characterLayoutEngine, &CharacterLayoutEngine::decrementAttackAmountClicked,
            this, &InitiativeDelegate::decrementAttackAmount);

    connect(characterLayoutEngine, &CharacterLayoutEngine::incrementAttackAmountClicked,
            this, &InitiativeDelegate::incrementAttackAmount);
    
    connect(characterLayoutEngine, &CharacterLayoutEngine::decrementWeaponSpeedClicked,
            this, &InitiativeDelegate::decrementWeaponSpeed);

    connect(characterLayoutEngine, &CharacterLayoutEngine::incrementWeaponSpeedClicked,
            this, &InitiativeDelegate::incrementWeaponSpeed);

    connect(characterLayoutEngine, &CharacterLayoutEngine::attackSubmitClicked,
            this, &InitiativeDelegate::attackSubmitted);

    // Misc action dropdown

    connect(characterLayoutEngine, QOverload<const QModelIndex&, Cyrus::MiscActionType>::of(&CharacterLayoutEngine::iconSelectorClicked),
            this,                  QOverload<const QModelIndex&, Cyrus::MiscActionType>::of(&InitiativeDelegate::iconSelected));

    connect(characterLayoutEngine, &CharacterLayoutEngine::miscSubmitClicked,
            this, &InitiativeDelegate::miscSubmitted);

    //attack action layout engine
    auto* attackActionLayoutEngine = static_cast<AttackActionLayoutEngine*>(LayoutFactory::forCharacterType<AttackAction>());

    connect(attackActionLayoutEngine, &AttackActionLayoutEngine::deleteItemClicked,
            this, &InitiativeDelegate::deleteItem);

    // cast action layout engine

    auto* castActionLayoutEngine = static_cast<CastActionLayoutEngine*>(LayoutFactory::forCharacterType<CastAction>());

    connect(castActionLayoutEngine, &CastActionLayoutEngine::deleteItemClicked,
            this, &InitiativeDelegate::deleteItem);

    // misc action layout engine

    auto* miscActionLayoutEngine = static_cast<MiscActionLayoutEngine*>(LayoutFactory::forCharacterType<MiscAction>());

    connect(miscActionLayoutEngine, &MiscActionLayoutEngine::deleteItemClicked,
            this, &InitiativeDelegate::deleteItem);
}


#include <QDebug>
#include <typeinfo>

void InitiativeDelegate::paint(QPainter* painter,
                               const QStyleOptionViewItem& option,
                               const QModelIndex& index) const
{
    painter->save();

    // character payload
    const QVariant charVar = index.data(Cyrus::CharacterRole);
    if (!charVar.canConvert<std::shared_ptr<Character>>()) { 
        painter->restore(); 
        return; 
    }

    auto character = charVar.value<std::shared_ptr<Character>>();
    if (!character) {
        painter->restore();
        return;
    }

    // local vars
    const LayoutSpec& spec = character->layoutSpec();
    const CastState castState = characterController_->stateFor(character->uuid());
    const Cyrus::CombatSequenceState state = characterController_->combatSequenceState();

    // State checks
    bool isSelectedIndex     = checkSelectedIndex(index, option, state, character);
    bool isCurrentInitiative = checkCurrentInitiative(index, character->initiative());
    bool isHovered           = checkHovered(option);
    bool isExpanded          = checkExpanded(isSelectedIndex, character);
    bool isCharacter         = checkIfCharacter(character);


    QPoint localCursor = cursorPosInItem(option);

    const auto& engine = LayoutFactory::forCharacter(*character);

    // Use static layout engine
    
    const auto layout = engine->calculate(option.rect, character, isSelectedIndex, isExpanded, state);

    // paint background
    QColor bg = backgroundForState(state, character, isSelectedIndex, isCurrentInitiative);

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);
    painter->setBrush(bg);
    painter->drawRoundedRect(layout->baseRect, spec.cornerRadius, spec.cornerRadius);

    // --- Paint the layout ---
    engine->paintLayout(
        painter, 
        layout, 
        character, 
        castState, 
        isSelectedIndex, 
        isExpanded, 
        localCursor,
        state
    ); 

    painter->restore();
}

bool InitiativeDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
                                     const QStyleOptionViewItem &option,
                                     const QModelIndex &index)
{
    if (event->type() == QEvent::MouseMove) {
        if (option.widget)
            const_cast<QWidget*>(option.widget)->update(option.rect);
    }

    if (event->type() == QEvent::MouseButtonRelease) {

        auto* view = qobject_cast<QListView*>(const_cast<QWidget*>(option.widget)); 

        const QVariant charVar = index.data(Cyrus::CharacterRole);
        if (!charVar.canConvert<std::shared_ptr<Character>>()) { return false; }
        auto character = charVar.value<std::shared_ptr<Character>>();

        const Cyrus::CombatSequenceState state = characterController_->combatSequenceState();

        bool isSelectedIndex = checkSelectedIndex(index, option, state, character);
        bool isExpanded      = checkExpanded(isSelectedIndex, character);
        QPoint localCursor   = cursorPosInItem(option, event);

        QUuid id = character->uuid();
        auto clamp = [](int v, int lo, int hi){ return std::max(lo, std::min(hi, v)); };

        auto* engine = LayoutFactory::forCharacter(*character);
        auto layout = engine->calculate(option.rect, character, isSelectedIndex, isExpanded, state);
        auto callback = engine->hitTest(index, layout, character, localCursor, state);
        if(callback){
            callback->execute(view);
            if (option.widget) const_cast<QWidget*>(option.widget)->update(option.rect);
            return true;
        }
    }

    return false;
}

QWidget* InitiativeDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex& index) const {
    qDebug() << "createEditor";
    
    if (index.data(Cyrus::ModelRoles::RoleTypeRole).toInt() != Cyrus::SpellNameEditRole)
        return nullptr;

    //in this editor I set the stylesheet but a lot of the styling isnt applied
    auto* editor = new QLineEdit(parent);
    editor->setStyleSheet(StyleRepository::lineEdit());
    editor->setAutoFillBackground(true); // Make the lineedit paint its own background

    // --- Update spell name on each keystroke ---
    connect(editor, &QLineEdit::textChanged, this, [this, editor, index](const QString& text) {
        auto charVar = index.data(Cyrus::CharacterRole);
        if (!charVar.canConvert<std::shared_ptr<Character>>()) return;
        auto character = charVar.value<std::shared_ptr<Character>>();

        emit spellNameEdited(character->uuid(), text);
    });

    // Commit on Enter
    connect(editor, &QLineEdit::returnPressed, this, [this, editor]() {
        emit commitData(editor);
        emit closeEditor(editor);
    });

    // Commit on focus loss
    connect(editor, &QLineEdit::editingFinished, this, [this, editor]() {
        emit commitData(editor);
        emit closeEditor(editor);
    });

    return editor;
}

void InitiativeDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
    qDebug() << "setEditorData";
    auto* lineEdit = qobject_cast<QLineEdit*>(editor);
    if (!lineEdit) return;

    auto charVar = index.data(Cyrus::CharacterRole);
    if (!charVar.canConvert<std::shared_ptr<Character>>()) return;

    auto character = charVar.value<std::shared_ptr<Character>>();
    auto state = characterController_->stateFor(character->uuid());
    lineEdit->setPlaceholderText(state.defaultText);
    lineEdit->setText(state.spellName);
}

void InitiativeDelegate::setModelData(QWidget* editor, QAbstractItemModel*, const QModelIndex& index) const {
    qDebug() << "setModelData";
    auto* lineEdit = qobject_cast<QLineEdit*>(editor);
    if (!lineEdit) return;

    //get character payload
    auto charVar = index.data(Cyrus::CharacterRole);
    if (!charVar.canConvert<std::shared_ptr<Character>>()) return;
    auto character = charVar.value<std::shared_ptr<Character>>();

    //get new name
    QString newName = lineEdit->text();

    //notify character controller to update cast state
    emit spellNameEdited(character->uuid(), newName);
}

void InitiativeDelegate::updateEditorGeometry(QWidget* editor,
                                              const QStyleOptionViewItem& option,
                                              const QModelIndex& index) const {

    // get char payload
    const QVariant charVar = index.data(Cyrus::CharacterRole);
    if (!charVar.canConvert<std::shared_ptr<Character>>()) { return; }
    auto character = charVar.value<std::shared_ptr<Character>>();

    const Cyrus::CombatSequenceState state = characterController_->combatSequenceState();

    // populate bools
    bool isSelectedIndex = checkSelectedIndex(index, option, state, character);
    bool isExpanded    = checkExpanded(isSelectedIndex, character);

    // calculate rects
    const auto layout = LayoutFactory::forCharacter(*character)->calculate(option.rect, character, isSelectedIndex, isExpanded);
    // needs to be casted to character layout
    const auto cLayout = std::static_pointer_cast<CharacterLayout>(layout);

    // Use your layout rect for the spell edit box
    if(cLayout->cast && editor) editor->setGeometry(cLayout->cast->spellEdit);
}


QSize InitiativeDelegate::sizeHint(const QStyleOptionViewItem& option,
                                   const QModelIndex& index) const {

    std::shared_ptr<Character> character;
    const QVariant charVar = index.data(Cyrus::CharacterRole);
    if (!charVar.canConvert<std::shared_ptr<Character>>()) { 
        //Mock character for size hint when the list is empty
        character = std::make_shared<Character>("SIZE_HINT_DEFAULT", 10, 0, 0, Cyrus::CharacterType::Fighter, Cyrus::ActionType::Cast);
    }
    character = charVar.value<std::shared_ptr<Character>>();

    const Cyrus::CombatSequenceState state = characterController_->combatSequenceState();

    const int width      = LayoutFactory::forCharacter(*character)->minimumWidth(character);
    bool isSelectedIndex = checkSelectedIndex(index, option, state, character);
    bool isExpanded      = checkExpanded(isSelectedIndex, character);
    const int preferredHeight = character->layoutSpec().preferredHeight;
    int height = isExpanded ? 2 * preferredHeight : preferredHeight;
    return QSize(width, height);
}


bool InitiativeDelegate::checkSelectedIndex(
    const QModelIndex& index, 
    const QStyleOptionViewItem& option, 
    const Cyrus::CombatSequenceState state,
    const std::shared_ptr<Character>& character) const 
{
    // Helper: has the character acted?
    bool notActed = !character->hasActed();

    switch (state) {
        case Cyrus::CombatSequenceState::NPC_DETERMINATION:
            if (notActed && character->faction() == Cyrus::Faction::Red)
                return true;
            break;

        case Cyrus::CombatSequenceState::PLAYER_DETERMINATION:
            if (notActed && character->faction() == Cyrus::Faction::Blue)
                return true;
            break;

        case Cyrus::CombatSequenceState::INITIATIVE:
            if (notActed)
                return true;
            break;

        case Cyrus::CombatSequenceState::RESOLUTION:
            {
                auto v = qobject_cast<const InitiativeView*>(option.widget);
                return v && (v->activeIndex() == index);
            }
            break;

        default:
            break;
    }

    return false;
}

bool InitiativeDelegate::checkExpanded(bool isActiveIndex, std::shared_ptr<Character> character) const {
    //Only expand for characters(base class) not actions
    if (character && typeid(*character) == typeid(Character)) {
        return isActiveIndex && !character->hasActed() && character->actionType() != Cyrus::ActionType::Unset;
    }
    return false;
}

bool InitiativeDelegate::checkCurrentInitiative(const QModelIndex& index, const int characterInitiative) const {
    // get initiative payload
    const QVariant initiativeVar = index.data(Cyrus::ModelRoles::InitiativeRole);
    const int currentInitiative = initiativeVar.canConvert<int>() ? initiativeVar.value<int>() : 0;
    // true if the current items initiative is equal to the models current initiative
    return characterInitiative == currentInitiative;
}

bool InitiativeDelegate::checkHovered(const QStyleOptionViewItem &option) const {
    return (option.state & QStyle::State_MouseOver);
}

bool InitiativeDelegate::checkIfCharacter(std::shared_ptr<Character> character) const {
    return character && typeid(*character) == typeid(Character);
}

QPoint InitiativeDelegate::cursorPosInItem(const QStyleOptionViewItem& option, const QEvent* event) const {
    if (!option.widget) {
        return QPoint(-9999, -9999);
    }
    if (event) {
        if (const auto* mouseEvent = dynamic_cast<const QMouseEvent*>(event)) {
            return mouseEvent->pos();
        }
    }
    return option.widget->mapFromGlobal(QCursor::pos());
}

// --- Background helper ---
QColor InitiativeDelegate::backgroundForState(const Cyrus::CombatSequenceState state,
                                 const std::shared_ptr<Character>& character,
                                 bool isActiveIndex,
                                 bool isCurrentInitiative) const {
    switch (state) {
        case Cyrus::CombatSequenceState::NPC_DETERMINATION: {
            if (character->faction() != Cyrus::Faction::Blue) {
                qDebug() << "NPC for npc determination";
                // gray out the background if thheir action is already submitted
                return character->hasActed() ? ColorRepository::segmentBackground() : ColorRepository::selectedBackground();
            }
            return Qt::transparent;
        }

        case Cyrus::CombatSequenceState::PLAYER_DETERMINATION: {
            if (character->faction() == Cyrus::Faction::Blue) {
                qDebug() << "PC for PC determination";
                // gray out the background if thheir action is already submitted
                return character->hasActed() ? ColorRepository::segmentBackground() : ColorRepository::selectedBackground();
            }
            return Qt::transparent;
        }

        case Cyrus::CombatSequenceState::INITIATIVE: {
            qDebug() << "Initiative state";
            return ColorRepository::selectedBackground();
        }

        case Cyrus::CombatSequenceState::RESOLUTION: {
            qDebug() << "Resolution state";
            if (isActiveIndex) return ColorRepository::selectedBackground();
            if (isCurrentInitiative) return ColorRepository::segmentBackground();
            return ColorRepository::baseBackground();
        }
    }

    // Safety fallback
    return Qt::transparent;
}

