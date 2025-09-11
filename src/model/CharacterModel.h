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

    void sort(int row = 0, Qt::SortOrder order = Qt::AscendingOrder) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Accessors
    std::shared_ptr<Character> getItem(const QModelIndex &index);
    QVector<std::shared_ptr<Character>> getItems() const;

    void setItem(const QModelIndex &index, std::shared_ptr<Character> character);

    // Add/remove
    void appendItem(QString name, int initiative, Cyrus::CharacterType characterType);
    void appendItem(std::shared_ptr<Character> rhs);
    void addItems(const QVector<std::shared_ptr<Character>>& characters);
    void removeItem(const QModelIndex& index);
    void clear();

    // Initiative/segment control
    void advanceSegment(int segment);
    void backtrackSegment(int segment);
    void advanceRound();
    void backtrackRound();

signals:
    void segmentUpdate(int segment);
    void roundUpdate(int round);

private:
    QVector<std::shared_ptr<Character>> items_;
    int segment_ = 0;
    int round_   = 1;
};
