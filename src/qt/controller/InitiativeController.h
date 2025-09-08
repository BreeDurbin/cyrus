#pragma once
#include <QObject>
#include <QHash>
#include <QModelIndex>
#include "InitiativeItemController.h"

class InitiativeController : public QObject {
    Q_OBJECT
public:
    explicit InitiativeController(QObject* parent = nullptr) : QObject(parent) {}

    InitiativeItemController& stateFor(const QModelIndex& index) {
        return states_[index];
    }

signals:
    void stateChanged(const QModelIndex& index);

public slots:
    void setSpellName(const QModelIndex& index, const QString& name) {
        auto& s = states_[index];
        s.spellName = name;
        emit stateChanged(index);
    }

    void incrementDuration(const QModelIndex& index, int delta) {
        auto& s = states_[index];
        s.duration = std::max(0, s.duration + delta);
        emit stateChanged(index);
    }

    void toggleExpanded(const QModelIndex& index) {
        auto& s = states_[index];
        s.expanded = !s.expanded;
        emit stateChanged(index);
    }

private:
    QHash<QUuid, InitiativeItemController> states_;
};