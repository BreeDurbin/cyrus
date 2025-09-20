#pragma once

#include <QAbstractListModel>
#include <QVector>
#include <memory>
#include "Enums.h"

class Character;

class CharacterModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit CharacterModel(QObject* parent = nullptr);

    // Required model overrides
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    void sort(int row = 0, Cyrus::CombatSequenceState state = Cyrus::CombatSequenceState::RESOLUTION);
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Accessors
    QModelIndex indexFromCharacter(const std::shared_ptr<Character>& character) const;
    std::shared_ptr<Character> getItem(const QModelIndex &index);
    QVector<std::shared_ptr<Character>> getItems(Cyrus::Faction faction) const;
    QVector<std::shared_ptr<Character>> getItems() const;

    void setItem(const QModelIndex &index, std::shared_ptr<Character> character);

    // Add/remove
    void appendItem(std::shared_ptr<Character> rhs);
    void addItems(const QVector<std::shared_ptr<Character>>& characters);
    void removeItem(const QModelIndex& index);
    void clear();

    // Initiative/segment control
    void advanceInitiative(int initiative);
    void backtrackInitiative(int initiative);
    void advanceRound();
    void backtrackRound();

signals:
    void roundUpdate(int round);

private:
    QVector<std::shared_ptr<Character>> items_;
    QHash<std::shared_ptr<Character>, int> rowMap_;
    QMultiHash<Cyrus::Faction, std::shared_ptr<Character>> factionMap_;

    int initiative_ = 0;
    int round_   = 1;
};
