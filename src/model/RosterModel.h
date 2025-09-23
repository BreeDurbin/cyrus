#pragma once

#include <QAbstractListModel>
#include <QVector>
#include <memory>
#include "Enums.h"

class Character;

class RosterModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit RosterModel(QObject* parent = nullptr);

    // Required model overrides
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Accessors
    std::shared_ptr<Character> getItem(const QModelIndex &index);
    QVector<std::shared_ptr<Character>> getItems(Cyrus::Faction faction) const;
    QVector<std::shared_ptr<Character>> getItems() const;
    void setItem(const QModelIndex &index, std::shared_ptr<Character> character);
    void appendItem(std::shared_ptr<Character> rhs);
    void addItems(const QVector<std::shared_ptr<Character>>& characters);
    void removeItem(const QModelIndex& index);
    void clear();

private:
    QVector<std::shared_ptr<Character>> items_;
};
