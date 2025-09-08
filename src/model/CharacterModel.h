#pragma once

#include <QAbstractListModel>
#include <QVector>
#include <memory>
#include "Character.h"
#include "shared/Enums.h"

class CharacterModel : public QAbstractListModel
{
    Q_OBJECT
public:
    using QAbstractListModel::QAbstractListModel;

    CharacterModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    void setFlags(const QModelIndex& index, const Qt::ItemFlags &flags);
    void sort(int /*column*/, Qt::SortOrder order = Qt::AscendingOrder) override;

    CharacterModel get(const QModelIndex& index) const;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    QHash<int, QByteArray> roleNames() const override;
    Character* getItem(const QModelIndex& index);
    QVector<Character> getItems() const;
    void setItem(const QModelIndex &index, const Character& character);
    void appendItem(QString name, int initiative, Cyrus::CharacterType characterType);
    void appendItem(Character rhs);
    void addItems(const QVector<Character>& characters);

    void advanceSegment(int segment);
    void backtrackSegment(int segment);
    void advanceRound();
    void backtrackRound();

    int segment() const { return segment_; };
    int round() const {return round_; };

    enum { 
        CharacterRole = Qt::UserRole + 1, 
        SegmentRole = Qt::UserRole + 2 
    };

signals:
    void segmentUpdate(int segment) const;
    void roundUpdate(int round) const;

private:
    QVector<Character> items_;
    int segment_{1};
    int round_{1};

};