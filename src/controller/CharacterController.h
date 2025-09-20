#pragma once
#include <QUuid>
#include <QObject>
#include <QHash>
#include <QModelIndex>
#include "CastState.h"
#include "Enums.h"
#include <memory>

class InitiativeView;
class CharacterModel;
class Character;

class CharacterController : public QObject {
    Q_OBJECT
public:
    CharacterController(InitiativeView* initiativeView,
                        CharacterModel* initiativeOrderModel,
                        CharacterModel* combatLogModel,
                        CharacterModel* rosterModel,
                        QObject* parent = nullptr);


    CharacterModel* initiativeModel() const { return initiativeOrderModel_; }
    CharacterModel* combatLogModel() const { return combatLogModel_; }
    CharacterModel* rosterModel() const { return rosterModel_; }
    CastState stateFor(const QUuid& id) {
        return states_.value(id);
    }
    Cyrus::CombatSequenceState combatSequenceState(){ return combatSequenceState_; }
    bool isRosterModified() const { return isRosterModified_; }
    std::string currentRosterName() const { return currentRosterName_; }
    void setGroupInitiative(const bool toggled) { isGroupInitiative_ = toggled; }
    bool isGroupInitiative() const { return isGroupInitiative_; }
    
signals:
    void stateChanged(const QUuid& id); // not sure if I need this currently tbd
    void requestScrollCombatLogToBottom();
    void roundChangedText(const QString& text);
    void combatSequenceStateChanged(Cyrus::CombatSequenceState state);
    void currentInitiativeChanged(const QModelIndex& index);

public slots:
    // Combat Sequence state
    void nextCombatSequenceState();
    void previousCombatSequenceState();

    // Character handlers
    void updateAction(const QModelIndex& index, Cyrus::ActionType actionType);
    void deleteItem(const QModelIndex& index);
    void cancelAction(const QModelIndex& index);
    void decrementInitiative(const QModelIndex& index);
    void incrementInitiative(const QModelIndex& index);

    // Cast Action handlers
    void setSpellName(const QUuid& id, const QString& name);
    void decrementCastingTime(const QUuid& id);
    void incrementCastingTime(const QUuid& id);
    void decrementDuration(const QUuid& id);
    void incrementDuration(const QUuid& id);
    void castSubmitted(const QUuid& id, const std::shared_ptr<Character>& character);

    //Attack Action handlers
    void decrementAttackAmount(const QModelIndex& index);
    void incrementAttackAmount(const QModelIndex& index);
    void decrementWeaponSpeed(const QModelIndex& index);
    void incrementWeaponSpeed(const QModelIndex& index);
    void attackSubmitted(const QModelIndex& index);

    // Misc action handlers
    void updateAction(const QModelIndex& index, Cyrus::MiscActionType miscActionType);
    void miscSubmitted(const QModelIndex& index);

    // button handlers
    QModelIndex addToRoster(const QString &name, Cyrus::CharacterType type);
    void advanceInitiative();
    void backtrackInitiative();
    void addRosterToInitiative();
    void addRosterMemberToInitiative(const QModelIndex& index);
    void saveRoster(const std::string& roster);
    QModelIndex loadRoster(const std::string& roster);
    void clearInitiativeOrder();
    void clearCombatLog();

    //Roster delegate
    void nextFaction(const QModelIndex& index);
    void previousFaction(const QModelIndex& index);
    void cloneRosterMember(const QModelIndex& index);
    void deleteRosterMember(const QModelIndex& index);

private:
    Cyrus::CombatSequenceState combatSequenceState_ = Cyrus::CombatSequenceState::NPC_DETERMINATION;
    QHash<QUuid, CastState> states_; 
    bool isRosterModified_{false};
    bool isGroupInitiative_{false};
    std::string currentRosterName_{"Unset"};
    InitiativeView* initiativeView_;
    CharacterModel* initiativeOrderModel_;
    CharacterModel* combatLogModel_;
    CharacterModel* rosterModel_;

    // helpers
    bool progressAction(const QModelIndex& index, const std::shared_ptr<Character> character);
    QModelIndex getNextIndex(const QModelIndex& index, bool isRemoved);
};

