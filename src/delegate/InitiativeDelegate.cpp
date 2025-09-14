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

#include<iostream>

InitiativeDelegate::InitiativeDelegate(CharacterController* characterController, QObject *parent) 
    : QStyledItemDelegate(parent), characterController_{characterController} {

    //get layout engines
    auto* characterLayoutEngine = static_cast<CharacterLayoutEngine*>(LayoutFactory::forCharacterType<Character>());
    //const auto& castActionLayoutengine = LayoutFactory::forCharacterType<CastAction>();   //need this later
    //const auto& attackActionLayoutEngine = LayoutFactory::forCharacterType<AttackAction>();  // need this later

    //Chain character Layout Engine signals to initiative delegate signals
    //create a dummy csharacter for the factory sso it will return a car layout engine
    connect(characterLayoutEngine, &CharacterLayoutEngine::spellNameEdited,
            this, &InitiativeDelegate::spellNameEdited);

    connect(characterLayoutEngine, &CharacterLayoutEngine::decrementCastingTimeClicked,
            this, &InitiativeDelegate::decrementCastingTimeClicked);

    connect(characterLayoutEngine, &CharacterLayoutEngine::incrementCastingTimeClicked,
            this, &InitiativeDelegate::incrementCastingTimeClicked);

    connect(characterLayoutEngine, &CharacterLayoutEngine::decrementDurationClicked,
            this, &InitiativeDelegate::decrementDurationClicked);

    connect(characterLayoutEngine, &CharacterLayoutEngine::incrementDurationClicked,
            this, &InitiativeDelegate::incrementDurationClicked);

    connect(characterLayoutEngine, &CharacterLayoutEngine::castSubmitted,
            this, &InitiativeDelegate::castSubmitted);

    connect(characterLayoutEngine, &CharacterLayoutEngine::deleteItemClicked,
            this, &InitiativeDelegate::deleteItemClicked);

    connect(characterLayoutEngine, &CharacterLayoutEngine::iconSelectorClicked,
            this, &InitiativeDelegate::iconSelectorClicked);

}


void InitiativeDelegate::paint(QPainter* painter,
                               const QStyleOptionViewItem& option,
                               const QModelIndex& index) const
{
    painter->save();

    const QVariant charVar = index.data(Cyrus::CharacterRole);
    if (!charVar.canConvert<std::shared_ptr<Character>>()) { painter->restore(); return; }
    auto character = charVar.value<std::shared_ptr<Character>>();
    const Character::LayoutSpec& spec = character->layoutSpec();
    const CastState castState = characterController_->stateFor(character->uuid());

    // State checks
    bool isActiveIndex    = checkActiveIndex(index, option);
    bool isCurrentSegment = checkCurrentSegment(index, character->initiative());
    bool isHovered        = checkHovered(option);
    bool isExpanded       = checkExpanded(isActiveIndex, character);
    bool isCharacter      = checkIfCharacter(character);

    QPoint localCursor = cursorPosInItem(option);

    const auto& engine = LayoutFactory::forCharacter(*character);

    // Use static layout engine
    const auto layout = engine->calculate(option.rect, character, isActiveIndex, isExpanded);

    // --- Background ---
    QColor bg = isCurrentSegment
        ? ColorRepository::selectedBackground()
        : ColorRepository::baseBackground();

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);
    painter->setBrush(bg);
    painter->drawRoundedRect(layout->baseRect, spec.cornerRadius, spec.cornerRadius);

    // --- Hover outline ---
    if (isHovered || isActiveIndex) {
        QPen hoverPen(ColorRepository::hoverOutlineBrush(layout->baseRect), 1.5);
        painter->setPen(hoverPen);
        painter->setBrush(Qt::NoBrush);
        painter->drawRoundedRect(layout->baseRect.adjusted(-3, -3, 3, 3),
                                 spec.cornerRadius, spec.cornerRadius);
    }

    // --- Paint the layout ---
    engine->paintLayout(
        painter, 
        layout, 
        character, 
        castState, 
        isActiveIndex, 
        isExpanded, 
        localCursor
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
        const QVariant charVar = index.data(Cyrus::CharacterRole);
        if (!charVar.canConvert<std::shared_ptr<Character>>()) { return false; }
        auto character = charVar.value<std::shared_ptr<Character>>();

        bool isActiveIndex = checkActiveIndex(index, option);
        bool isExpanded    = checkExpanded(isActiveIndex, character);
        QPoint localCursor = cursorPosInItem(option, event);

        QUuid id = character->uuid();
        auto clamp = [](int v, int lo, int hi){ return std::max(lo, std::min(hi, v)); };

        auto* engine = LayoutFactory::forCharacter(*character);
        auto layout = engine->calculate(option.rect, character, isActiveIndex, isExpanded);
        auto callback = engine->hitTest(index, layout, character, localCursor);
        if(callback){
            callback->execute();
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

    // populate bools
    bool isActiveIndex = checkActiveIndex(index, option);
    bool isExpanded    = checkExpanded(isActiveIndex, character);

    // calculate rects
    const auto layout = LayoutFactory::forCharacter(*character)->calculate(option.rect, character, isActiveIndex, isExpanded);
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
        character = std::make_shared<Character>("SIZE_HINT_DEFAULT", 10, Cyrus::CharacterType::Fighter, Cyrus::ActionType::Cast);
    }
    character = charVar.value<std::shared_ptr<Character>>();

    const int width     = LayoutFactory::forCharacter(*character)->minimumWidth(character);
    bool isActiveIndex = checkActiveIndex(index, option);
    bool isExpanded    = checkExpanded(isActiveIndex, character);
    const int preferredHeight = character->layoutSpec().preferredHeight;
    int height = isExpanded ? 2 * preferredHeight : preferredHeight;
    return QSize(width, height);
}


bool InitiativeDelegate::checkActiveIndex(const QModelIndex& index, const QStyleOptionViewItem& option) const {
    auto v = qobject_cast<const InitiativeView*>(option.widget);
    bool result = v && (v->activeIndex() == index);
    //qDebug() << "checkActiveIndex: row" << index.row() << "active?" << result;
    return result;
}

bool InitiativeDelegate::checkExpanded(bool isActiveIndex, std::shared_ptr<Character> character) const {
    //Only expand for characters(base class) not actions
    if (character && typeid(*character) == typeid(Character)) {
        return isActiveIndex && character->actionType() != Cyrus::ActionType::None;
    }
    return false;
}

bool InitiativeDelegate::checkCurrentSegment(const QModelIndex& index, const int currentInitiative) const {
    const QVariant segmentVar = index.data(Cyrus::ModelRoles::SegmentRole);
    const int currentSegment = !segmentVar.canConvert<int>() ? 0 : segmentVar.value<int>();
    return currentInitiative == currentSegment;
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
