#include "CharacterController.h"
#include "CharacterModel.h"
#include "Character.h"
#include "InitiativeView.h"
#include "CastAction.h"
#include "TomlHelper.h"
#include "Enums.h"
#include "CastState.h"


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

    connect(initiativeOrderModel_, &CharacterModel::segmentUpdate,
            this, [this](int segment) {
                emit segmentChangedText(
                    QString("Segment: %1 / 10").arg(segment));
            });

    connect(initiativeOrderModel_, &CharacterModel::roundUpdate,
            this, [this](int round) {
                emit roundChangedText(
                    QString("Round: %1").arg(round));
            });

    qDebug() << "CharacterController initialized.";
}


//update model
void CharacterController::updateAction(const QModelIndex& index, Cyrus::ActionType actionType) {
    std::shared_ptr<Character> character = initiativeOrderModel_->getItem(index);
    if (!character) return;
    // untoggle if same icon is clicked a second time
    if (character->actionType() == actionType) character->setActionType(Cyrus::ActionType::None);
    else character->setActionType(actionType);
    //invalid use of incomplete type ‘class CharacterModel’ here
    emit initiativeOrderModel_->dataChanged(index, index, {Cyrus::CharacterRole});
}

void CharacterController::deleteItem(const QModelIndex& index) {
    //for now just assume the item to be deleted is in initiative model
    initiativeOrderModel_->removeRow(index.row());
}

// update controller cached state

void CharacterController::setSpellName(const QUuid& id, const QString& name) {
    qDebug() << "CharacterController setting spell name.";
    auto& castState = states_[id];
    castState.spellName = name;
}

void CharacterController::decrementCastingTime(const QUuid& id) {
    qDebug() << "Decrementing casting time.";
    auto& castState = states_[id];
    castState.castingTime = std::max(0, castState.castingTime - 1);
}

void CharacterController::incrementCastingTime(const QUuid& id) {
    qDebug() << "Incrementing casting time.";
    auto& castState = states_[id];
    castState.castingTime = std::min(10, castState.castingTime + 1);
}

void CharacterController::decrementDuration(const QUuid& id) {
    qDebug() << "Decrementing duration.";
    auto& castState = states_[id];
    castState.duration = std::max(0, castState.duration - 1);
}

void CharacterController::incrementDuration(const QUuid& id) {
    qDebug() << "Incrementing duration.";
    auto& castState = states_[id];
    castState.duration = std::min(99, castState.duration + 1);
}

void CharacterController::castSubmitted(const QUuid& id, const Character& character){
    CastState castState = states_[id];

    // Create a new spell action card cast by the submitting char
    std::shared_ptr<CastAction> castAction = std::make_shared<CastAction>(character.name(), character.initiative(), 
            character.characterType(), castState.spellName, castState.castingTime, castState.duration);
    initiativeOrderModel_->appendItem(castAction);
    initiativeOrderModel_->sort();
    states_.remove(id);
}


// Update initiative of rostered characters

void CharacterController::incrementRosterMemberInitiative(const QModelIndex& index) {
    qDebug() << "Incrementing initiative.";
    auto character = rosterModel_->getItem(index);
    character->setInitiative(std::min(10, character->initiative() + 1));
}

void CharacterController::decrementRosterMemberInitiative(const QModelIndex& index) {
    qDebug() << "Decrementing initiative.";
    auto character = rosterModel_->getItem(index);
    character->setInitiative(std::max(0, character->initiative() - 1));
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
    initiativeOrderModel_->sort(0);

    // Ensure something is selected
    if (!initiativeView_->hasActiveIndex() && initiativeOrderModel_->rowCount() > 0) {
        QModelIndex first = initiativeOrderModel_->index(0, 0);
        initiativeView_->setActiveIndex(first);
        // set the starting segment to the initiative of the character first in the order
        initiativeOrderModel_->advanceSegment(initiativeOrderModel_->getItem(first)->initiative());
    }
}

void CharacterController::addRosterMemberToInitiative(const QModelIndex& index) {
    if (std::shared_ptr<Character> character = rosterModel_->getItem(index)) {
        qDebug() << "Adding roster member to initiative: " << character->name();
        initiativeOrderModel_->appendItem(character);
        initiativeOrderModel_->sort(0);

        if (!initiativeView_->hasActiveIndex() && initiativeOrderModel_->rowCount() > 0) {
            QModelIndex first = initiativeOrderModel_->index(0, 0);
            initiativeView_->setActiveIndex(first);
            // set the starting segment to the initiative of the character first in the order
            initiativeOrderModel_->advanceSegment(initiativeOrderModel_->getItem(first)->initiative());
        }
    }
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

    bool removedCurrent = false;

    // --- Handle CastAction specifics ---
    if (auto castAction = std::dynamic_pointer_cast<CastAction>(character)) {
        if (castAction->duration() == 0) {
            initiativeOrderModel_->removeItem(currentIndex);
            removedCurrent = true;
        } else { castAction->passRound(); } // duration will decrement 
    }

    // --- Log the action ---
    combatLogModel_->appendItem(character);

    // --- Compute next active index ---
    int nextRow;
    if (removedCurrent) {
        // If the current row was removed, rows shifted up → reuse same row index
        nextRow = std::min(currentIndex.row(), initiativeOrderModel_->rowCount() - 1);
    } else {
        nextRow = currentIndex.row() + 1;
    }

    if (initiativeOrderModel_->rowCount() > 0) {
        // wrap around index to nextround if there are no more cards in the round
        if(nextRow >= initiativeOrderModel_->rowCount()) nextRow = 0;
           
        // if the next card has initiative > 10 pass round on the rest of the cards in the initiative
        QModelIndex nextIndex = initiativeOrderModel_->index(nextRow, 0);
        auto nextCharacter = initiativeOrderModel_->getItem(nextIndex);
        if(nextCharacter->initiative() > 10){
            // advance the round on all action cardss which move into the next round > sesgment 10 rolls over
            passRoundOnHighInitiative(nextIndex);
            //wrap around to the next round
            auto zerothIndex = initiativeOrderModel_->index(0, 0);
            initiativeView_->setActiveIndex(zerothIndex);
            int nextSegment = initiativeOrderModel_->getItem(zerothIndex)->initiative();
            initiativeOrderModel_->advanceSegment(nextSegment);
        }
        //otherwise advance the segment in the model and update the active index in the view
        else {
            initiativeView_->setActiveIndex(nextIndex);
            initiativeOrderModel_->advanceSegment(nextCharacter->initiative());
        }
    }
    // --- Scroll combat log ---
    emit requestScrollCombatLogToBottom();
}

void CharacterController::passRoundOnHighInitiative(const QModelIndex& index){
    //pass round on each card which is out of scope for the round // initiative() > 10 rolls over to next round
    for (int i = index.row(); i < initiativeOrderModel_->rowCount(); ++i) {
        if (auto castAction = std::dynamic_pointer_cast<CastAction>(initiativeOrderModel_->getItem(initiativeOrderModel_->index(i, 0)))) {
            if( castAction->initiative() > 10 ) castAction->passRound(); // 
        }
    }
    // Re-sort initiative order after updating effective initiatives
    initiativeOrderModel_->sort();
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
        initiativeOrderModel_->backtrackSegment(character->initiative());
    }

    initiativeView_->setActiveIndex(previousIndex);
}

QModelIndex CharacterController::addToRoster(const QString &name, int initiative, Cyrus::CharacterType type){
    rosterModel_->appendItem(name, initiative, type);
    // Index of newly added row
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
}

QModelIndex CharacterController::loadRoster(const std::string& roster) {

    // Clear the current roster in the controller
    rosterModel_->clear();

    qDebug() << "Loading roster... " << roster;

    const auto& characters = TomlHelper::loadRepository(roster + ".toml");
    qDebug() << "Loaded " << characters.size() << " characters.";

    rosterModel_->addItems(characters);
    return rosterModel_->index(rosterModel_->rowCount() - 1, 0);
}

void CharacterController::clearInitiativeOrder(){
    initiativeOrderModel_->clear();
    initiativeOrderModel_->segmentUpdate(0);
    initiativeOrderModel_->roundUpdate(1);
}

void CharacterController::clearCombatLog(){
    combatLogModel_->clear();
}