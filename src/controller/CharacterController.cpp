#include "CharacterController.h"
#include "CharacterModel.h"
#include "Character.h"
#include "InitiativeView.h"
#include "CastAction.h"
#include "TomlHelper.h"
#include "Enums.h"
#include "CastState.h"
#include <QTimer>
#include "AttackAction.h"
#include "MiscAction.h"
#include <algorithm>

CharacterController::CharacterController(InitiativeView* initiativeView,
                                        CharacterModel* initiativeOrderModel,
                                        CharacterModel* combatLogModel,
                                        CharacterModel* rosterModel,
                                        QObject* parent)
    : QObject(parent),
    initiativeView_(initiativeView),
    initiativeOrderModel_(initiativeOrderModel),
    combatLogModel_(combatLogModel),
    rosterModel_(rosterModel)
{
    // 🔍 Optional sanity checks
    Q_ASSERT(initiativeView_);
    Q_ASSERT(initiativeOrderModel_);
    Q_ASSERT(combatLogModel_);
    Q_ASSERT(rosterModel_);

    connect(initiativeOrderModel_, &CharacterModel::roundUpdate,
            this, [this](int round) {
                emit roundChangedText(
                    QString("Round: %1").arg(round));
            });

    qDebug() << "CharacterController initialized.";
}


//update character model
void CharacterController::updateAction(const QModelIndex& index, Cyrus::ActionType actionType) {
    std::shared_ptr<Character> character = initiativeOrderModel_->getItem(index);
    if (!character) return;
    // untoggle if same icon is clicked a second time
    if (character->actionType() == actionType) character->setActionType(Cyrus::ActionType::Unset);
    else character->setActionType(actionType);
    //invalid use of incomplete type ‘class CharacterModel’ here
    emit initiativeOrderModel_->dataChanged(index, index, {Cyrus::CharacterRole});
}

void CharacterController::deleteItem(const QModelIndex& index) {
    if (!index.isValid()) return;

    // Advance initiative only if the deleted item is the active one
    QModelIndex active = initiativeView_->activeIndex();
    if (active.isValid() && active == index) {
        advanceInitiative();
    }

    initiativeOrderModel_->removeRow(index.row());
}

void CharacterController::cancelAction(const QModelIndex& index) {
    auto character = initiativeOrderModel_->getItem(index);
    if (!character) return;

    // 1. Reset acted state
    character->setActed(false);

    // 2. Remove any dependent rows (same uuid, not base class)
    const QUuid targetUuid = character->uuid();

    // We need to iterate backwards so row removals don’t invalidate subsequent indexes.
    for (int row = initiativeOrderModel_->rowCount() - 1; row > index.row(); --row) {
        QModelIndex idx = initiativeOrderModel_->index(row, 0);
        auto other = initiativeOrderModel_->getItem(idx);

        if (other && other->uuid() == targetUuid) {
            // Compare dynamic type to base character type
            if (typeid(*other) != typeid(*character)) {
                deleteItem(idx);
            }
        }
    }
}

void CharacterController::decrementInitiative(const QModelIndex& index) {
    qDebug() << "Decrementing initiative. Is group initiative? " << isGroupInitiative_;
    auto character = initiativeOrderModel_->getItem(index);
    if (!character) return;

    int initiative = character->initiative();
    initiative = std::clamp(initiative - 1, 1, 10);

    if(isGroupInitiative_){
        auto characters = initiativeOrderModel_->getItems(character->faction());
        for(auto c : characters){
            qDebug() << "setting initiative " << character->name() << " to " << initiative;
            c->setInitiative(initiative);
            QModelIndex idx = initiativeOrderModel_->indexFromCharacter(c);
            if (idx.isValid()) {
                emit initiativeOrderModel_->dataChanged(idx, idx, {Qt::DisplayRole});
            }
        }
        return;
    }
    character->setInitiative(initiative);
}

void CharacterController::incrementInitiative(const QModelIndex& index) {
    qDebug() << "Incrementing initiative. Is group initiative? " << isGroupInitiative_;
    auto character = initiativeOrderModel_->getItem(index);
    if (!character) return;

    int initiative = character->initiative();
    initiative = std::clamp(initiative + 1, 1, 10);

    if(isGroupInitiative_){
        auto characters = initiativeOrderModel_->getItems(character->faction());
        for(auto c : characters){
            c->setInitiative(initiative);
            QModelIndex idx = initiativeOrderModel_->indexFromCharacter(c);
            if (idx.isValid()) {
                emit initiativeOrderModel_->dataChanged(idx, idx, {Qt::DisplayRole});
            }
        }
        return;
    }
    character->setInitiative(initiative);
}


// cast actions

void CharacterController::setSpellName(const QUuid& id, const QString& name) {
    qDebug() << "CharacterController setting spell name.";
    auto& castState = states_[id]; // creates new CastState if id doesn't exist
    castState.spellName = name;
}

void CharacterController::decrementCastingTime(const QUuid& id) {
    qDebug() << "Decrementing casting time.";
    auto& castState = states_[id]; // creates new CastState if id doesn't exist
    castState.castingTime = std::max(0, castState.castingTime - 1);
}

void CharacterController::incrementCastingTime(const QUuid& id) {
    qDebug() << "Incrementing casting time.";
    auto& castState = states_[id]; // creates new CastState if id doesn't exist
    castState.castingTime = std::min(10, castState.castingTime + 1);
}

void CharacterController::decrementDuration(const QUuid& id) {
    qDebug() << "Decrementing duration.";
    auto& castState = states_[id]; // creates new CastState if id doesn't exist
    castState.duration = std::max(0, castState.duration - 1);
}

void CharacterController::incrementDuration(const QUuid& id) {
    qDebug() << "Incrementing duration.";
    auto& castState = states_[id]; // creates new CastState if id doesn't exist
    castState.duration = std::min(99, castState.duration + 1);
}

void CharacterController::castSubmitted(const QUuid& id, const std::shared_ptr<Character>& character){
    qDebug() << "Handling cast submitted.";

    if (!character) {
        qWarning() << "castSubmitted called with null character";
        return;
    }

    if (!states_.contains(id)) {
        qWarning() << "castSubmitted called for unknown id:" << id;
        return;
    }

    // Copy the state first
    auto castState = states_.value(id);

    auto castAction = std::make_shared<CastAction>(
        *character,
        castState.spellName,
        castState.castingTime,
        castState.duration
    );

    initiativeOrderModel_->appendItem(castAction);
    initiativeOrderModel_->sort(0, combatSequenceState_);
    states_.remove(id); 

    character->setActed(true);
}

// Attack actions

void CharacterController::decrementAttackAmount(const QModelIndex& index){
    auto character = initiativeOrderModel_->getItem(index);
    if (!character) return;
    qDebug() << "Decrementing attack rate for character: " << character->name();
    AttackRate ar = character->attackRate();
    character->setAttackRate(--ar);
}

void CharacterController::incrementAttackAmount(const QModelIndex& index){
    auto character = initiativeOrderModel_->getItem(index);
    if (!character) return;
    qDebug() << "Incrementing attack rate for character: " << character->name();
    AttackRate ar = character->attackRate();
    character->setAttackRate(++ar);
}

void CharacterController::decrementWeaponSpeed(const QModelIndex& index){
    auto character = initiativeOrderModel_->getItem(index);
    if (!character) return;
    qDebug() << "Decrementing Weapon Speed for character: " << character->name();
    int weaponSpeed = character->weaponSpeed();
    character->setWeaponSpeed(--weaponSpeed);
}

void CharacterController::incrementWeaponSpeed(const QModelIndex& index){
    auto character = initiativeOrderModel_->getItem(index);
    if (!character) return;
    qDebug() << "Incrementing Weapon Speed for character: " << character->name();
    int weaponSpeed = character->weaponSpeed();
    character->setWeaponSpeed(++weaponSpeed);
}

void CharacterController::attackSubmitted(const QModelIndex& index) {
    auto character = initiativeOrderModel_->getItem(index);
    auto attackRate = character->attackRate();

    for (int i = 1; i <= attackRate.attack(); ++i) {
        qDebug() << "Handling attack Submitted for character " << character->name() << " setting attack number " << i;
        auto attack = std::make_shared<AttackAction>(*character, i);
        qDebug() << "name: " << attack->name() << " attackNumber " << attack->attackNumber() << " weapon speed " << attack->weaponSpeed();
        initiativeOrderModel_->appendItem(attack);
    }

    character->setActed(true);

    initiativeOrderModel_->sort(index.row() + 1, combatSequenceState_);
}

// Misc actions

void CharacterController::updateAction(const QModelIndex& index, Cyrus::MiscActionType miscActionType){
    std::shared_ptr<Character> character = initiativeOrderModel_->getItem(index);
    if (!character) return;
    // untoggle if same icon is clicked a second time
    if (character->miscActionType() == miscActionType) character->setMiscActionType(Cyrus::MiscActionType::Unset);
    else character->setMiscActionType(miscActionType);
    emit initiativeOrderModel_->dataChanged(index, index, {Cyrus::CharacterRole});
}

void CharacterController::miscSubmitted(const QModelIndex& index) {
    if (!index.isValid())
        return;

    // Get the character at this index
    auto character = initiativeOrderModel_->getItem(index);
    if (!character)
        return;

    // Mark the character as having acted
    character->setActed(true);

    // Create a new MiscAction based on the character and the specified type
    auto miscAction = std::make_shared<MiscAction>(*character);

    // Optionally, set a specific scroll cost or other properties
    // miscAction->setScrollCost(...);  // if needed

    // Insert the MiscAction into the initiative order right after the original character
    initiativeOrderModel_->appendItem(miscAction);

    // Re-sort the initiative order starting from the next row
    int insertRow = index.row() + 1;
    initiativeOrderModel_->sort(insertRow, combatSequenceState_);
}



// Update faction of rostered characters

void CharacterController::nextFaction(const QModelIndex& index) {
    qDebug() << "Cycling to next faction.";
    auto character = rosterModel_->getItem(index);
    Cyrus::Faction nextFaction = Cyrus::nextFaction(character->faction());
    character->setFaction(nextFaction);
}

void CharacterController::previousFaction(const QModelIndex& index) {
    qDebug() << "Cycling to previous faction.";
    auto character = rosterModel_->getItem(index);
    Cyrus::Faction prevFaction = Cyrus::prevFaction(character->faction());
    character->setFaction(prevFaction);
}

// clone a rostered character
void CharacterController::cloneRosterMember(const QModelIndex& index){
    rosterModel_->appendItem(std::make_shared<Character>(rosterModel_->getItem(index)->clone()));
}

// delete a rostered character
void CharacterController::deleteRosterMember(const QModelIndex& index){
    rosterModel_->removeRow(index.row());
}

// Handlers triggered by buttons in the ui

void CharacterController::addRosterToInitiative() {
    // Loop over all rows in characterModel_
    for (int row = 0; row < rosterModel_->rowCount(); ++row) {
        QModelIndex idx = rosterModel_->index(row, 0);
        if (std::shared_ptr<Character> character = rosterModel_->getItem(idx)) {
            qDebug() << "Adding roster member to initiative: " << character->name();
            initiativeOrderModel_->appendItem(character);
        }
    }

    // Sort initiative after adding
    initiativeOrderModel_->sort(0, combatSequenceState_);

    // Ensure something is selected
    if (!initiativeView_->hasActiveIndex() && initiativeOrderModel_->rowCount() > 0) {
        QModelIndex first = initiativeOrderModel_->index(0, 0);
        initiativeView_->setActiveIndex(first);
        // set the starting segment to the initiative of the character first in the order
        initiativeOrderModel_->advanceInitiative(initiativeOrderModel_->getItem(first)->initiative());
    }
}

void CharacterController::addRosterMemberToInitiative(const QModelIndex& index) {
    if (std::shared_ptr<Character> character = rosterModel_->getItem(index)) {
        qDebug() << "Adding roster member to initiative: " << character->name();
        initiativeOrderModel_->appendItem(character);
        initiativeOrderModel_->sort(0, combatSequenceState_);

        if (!initiativeView_->hasActiveIndex() && initiativeOrderModel_->rowCount() > 0) {
            QModelIndex first = initiativeOrderModel_->index(0, 0);
            initiativeView_->setActiveIndex(first);
            // set the starting segment to the initiative of the character first in the order
            initiativeOrderModel_->advanceInitiative(initiativeOrderModel_->getItem(first)->initiative());
        }
    }
}

void CharacterController::nextCombatSequenceState(){
    int next = (static_cast<int>(combatSequenceState_) + 1) % 4;
    combatSequenceState_ = static_cast<Cyrus::CombatSequenceState>(next);
    initiativeOrderModel_->sort(0, combatSequenceState_);
    emit combatSequenceStateChanged(combatSequenceState_);
}

void CharacterController::previousCombatSequenceState(){
    int previous = (static_cast<int>(combatSequenceState_) - 1);
    // its ok to hard code this because no future sequence additions will ever happen. 
    // The combat sequence in adnd2e is set in stone in the phb
    //  once additional systems are supported this will need to be pulled out 
    // into a 2e specific subclass controllers
    if(previous < 0) previous = 3;
    combatSequenceState_ = static_cast<Cyrus::CombatSequenceState>(previous);
    initiativeOrderModel_->sort(0, combatSequenceState_);
    emit combatSequenceStateChanged(combatSequenceState_);
}


void CharacterController::advanceInitiative() {
    // Validation checks
    if (!initiativeView_->hasActiveIndex())
        return;

    QModelIndex currentIndex = initiativeView_->activeIndex();
    if (!currentIndex.isValid())
        return;

    int rowCount = initiativeOrderModel_->rowCount();
    if (rowCount == 0) return;

    // Get the current character
    std::shared_ptr<Character> character = initiativeOrderModel_->getItem(currentIndex);
    if (!character) return;

    // remove if the current action expires (atack finished, spell duration ended)
    bool isRemoved = progressAction(currentIndex, character);

    // --- Log the action ---
    combatLogModel_->appendItem(character);
    emit requestScrollCombatLogToBottom();

    // --- Compute next active index ---
    QModelIndex nextIndex = getNextIndex(currentIndex, isRemoved);

    // --- If there are any items left, move the active index to next and advance the models current initaitive to the next row --- 
    if (initiativeOrderModel_->rowCount() > 0) {
        auto nextCharacter = initiativeOrderModel_->getItem(nextIndex);
        initiativeView_->setActiveIndex(nextIndex);
        initiativeOrderModel_->advanceInitiative(nextCharacter->initiative());
        emit currentInitiativeChanged(nextIndex);
    }

}

bool CharacterController::progressAction(const QModelIndex& index, std::shared_ptr<Character> character){
    // --- Handle CastAction specifics ---
    if (auto castAction = std::dynamic_pointer_cast<CastAction>(character)) {
        if (castAction->duration() == 0) {
            initiativeOrderModel_->removeItem(index);
            return true;
        } else { 
            castAction->passRound(); // decrement duration
        }
    }

    // --- Handle AttackAction specifics ---
    if (auto attackAction = std::dynamic_pointer_cast<AttackAction>(character)) {
        initiativeOrderModel_->removeItem(index);
        return true;
    }

    // --- Handle Misc Magic item specifics ---
    if (auto miscAction = std::dynamic_pointer_cast<MiscAction>(character)) {
        initiativeOrderModel_->removeItem(index);
        return true;
    }
    return false;
}

// get next row, if deleted current item then that would be the current index, other wise do wrap around
QModelIndex CharacterController::getNextIndex(const QModelIndex& index, bool isRemoved){
    int next = isRemoved ? index.row() : ( index.row() + 1 ); // next index increment
    next = next >= initiativeOrderModel_->rowCount() ? 0 : next; //wrap around
    return initiativeOrderModel_->index(next, 0);
}

void CharacterController::backtrackInitiative() {
    if(!initiativeView_->hasActiveIndex()) return;

    //collapse the current row by setting character action type to none the current index has base 
    // type Character (derived class wont expand and its action type needs to stay the same)

    // if the current index is derived type CastAction : Character
    //      duration++;
    
    int previousRow = initiativeView_->activeIndex().row() - 1;
    if (previousRow < 0) {
        previousRow = initiativeOrderModel_->rowCount() - 1;
    }

    QModelIndex previousIndex = initiativeOrderModel_->index(previousRow, 0);

    if (std::shared_ptr<Character> character = initiativeOrderModel_->getItem(previousIndex)) {
        // Backtrack initiative segment
        initiativeOrderModel_->backtrackInitiative(character->initiative());
    }

    initiativeView_->setActiveIndex(previousIndex);
    emit currentInitiativeChanged(previousIndex);
}

QModelIndex CharacterController::addToRoster(const QString &name, Cyrus::CharacterType type){
    rosterModel_->appendItem(std::make_shared<Character>(name, 0, 2, 0, type));
    isRosterModified_ = true;
    return rosterModel_->index(rosterModel_->rowCount() - 1, 0);
}


void CharacterController::saveRoster(const std::string& roster) {

    qDebug() << "Saving roster ... " << roster;
    
    const auto characters = rosterModel_->getItems();
    if (characters.empty()) {
        qDebug() << "No characters to save. Saving failed.";
        return;
    }

    TomlHelper::saveRepository(roster + ".toml", characters);
    isRosterModified_ = false;
    currentRosterName_ = roster;
}

QModelIndex CharacterController::loadRoster(const std::string& roster) {

    // Clear the current roster in the controller
    rosterModel_->clear();

    // clear to prevent the num cache from just repeatedly counting up when roster loads happen repeatedly
    Character::clearNumberCache();

    qDebug() << "Loading roster... " << roster;

    const auto& characters = TomlHelper::loadRepository(roster + ".toml");
    qDebug() << "Loaded " << characters.size() << " characters.";

    rosterModel_->addItems(characters);
    isRosterModified_ = false;
    currentRosterName_ = roster;

    return rosterModel_->index(rosterModel_->rowCount() - 1, 0);
}

void CharacterController::clearInitiativeOrder(){
    initiativeOrderModel_->clear();
    initiativeOrderModel_->roundUpdate(1);
}

void CharacterController::clearCombatLog(){
    combatLogModel_->clear();
}