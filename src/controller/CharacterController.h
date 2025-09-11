#pragma once
#include <QUuid>
#include <QObject>
#include <QHash>
#include <QModelIndex>
#include "CastState.h"
#include "Enums.h"

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

    QModelIndex addToRoster(const QString &name, int initiative, Cyrus::CharacterType type);
    QModelIndex loadRoster();
    CastState& stateFor(const QUuid& id) {
        return states_[id];
    }

signals:
    void stateChanged(const QUuid& id); // not sure if I need this currently tbd
    void requestScrollCombatLogToBottom();
    void segmentChangedText(const QString& text);
    void roundChangedText(const QString& text);

public slots:
    // update controller cached state
    void setSpellName(const QUuid& id, const QString& name);
    void decrementCastingTime(const QUuid& id);
    void incrementCastingTime(const QUuid& id);
    void decrementDuration(const QUuid& id);
    void incrementDuration(const QUuid& id);
    void castSubmitted(const QUuid& id, const Character& caster);

    //update model
    void updateAction(const QModelIndex& index, Cyrus::ActionType actionType);
    void deleteItem(const QModelIndex& index);

    // button handlers
    void advanceInitiative();
    void passRoundOnHighInitiative(const QModelIndex& index);
    void backtrackInitiative();
    void addRosterToInitiative();
    void addRosterMemberToInitiative(const QModelIndex& index);
    void saveRoster(const std::string& roster);
    QModelIndex loadRoster(const std::string& roster);
    void clearInitiativeOrder();
    void clearCombatLog();

private:
    QHash<QUuid, CastState> states_; 
    InitiativeView* initiativeView_;
    CharacterModel* initiativeOrderModel_;
    CharacterModel* combatLogModel_;
    CharacterModel* rosterModel_;
};

