#include "CharacterModel.h"
#include "Character.h"
#include <QColor>
#include <QFont>
#include <QIcon>
#include "../style/ColorRepository.h"
#include <QAbstractItemView>
#include "../helper/Enums.h"

#include <iostream>

CharacterModel::CharacterModel(QObject* parent) : QAbstractListModel(parent) {  }

int CharacterModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 0;
    return items_.size();
}

QVariant CharacterModel::data(const QModelIndex &index, int role) const{
    if (index.row() < 0 || index.row() >= static_cast<int>(items_.size())) return QVariant();
    if (index.column() != 0) return QVariant();

    switch(role){
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::FontRole:
            return QFont("Helvetica", 14, QFont::Bold);
        //case Qt::BackgroundRole
            //return QBrush(ColorRepository::baseBackground());
        case Qt::ForegroundRole:
            return QBrush(ColorRepository::text());
        case Qt::DecorationRole:
            return QIcon(":/icon/initiative.png");
        case Qt::DisplayRole:
            return items_[index.row()].name();
        case Qt::SizeHintRole:
            return QSize(80, 50);
        case CharacterRole:
            return QVariant::fromValue(items_.at(index.row()));
        case SegmentRole:
            return QVariant::fromValue(segment_);
        default:
            return QVariant();
    }
}

QHash<int, QByteArray> CharacterModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[Qt::TextAlignmentRole] = "textAlignment";
    roles[Qt::FontRole]          = "font";
    roles[Qt::BackgroundRole]    = "background";
    roles[Qt::ForegroundRole]    = "foreground";
    roles[Qt::DecorationRole]    = "decoration";
    roles[Qt::DisplayRole]       = "display";
    roles[Qt::EditRole]          = "edit";
    roles[Qt::SizeHintRole]      = "sizeHint";

    // Custom roles
    roles[CharacterRole]         = "character";
    roles[SegmentRole]           = "segment";

    return roles;
}

Qt::ItemFlags CharacterModel::flags(const QModelIndex& index) const
{
    if(index.isValid()) return QAbstractListModel::flags(index);
    return Qt::NoItemFlags;
}

void CharacterModel::sort(int /*column*/, Qt::SortOrder order) {
    if(order == Qt::AscendingOrder){
        std::sort(items_.begin(), items_.end(), [](const Character& lhs, const Character& rhs){
            return lhs.initiative() < rhs.initiative();
        });
    } else {
        std::sort(items_.begin(), items_.end(), [](const Character& lhs, const Character& rhs){
            return lhs.initiative() > rhs.initiative();
        });
    }
    emit dataChanged(index(0, 0), index(items_.size() - 1, 0), {Qt::DisplayRole});
}

bool CharacterModel::removeRows(int row, int count, const QModelIndex &parent) {
    if (row < 0 || row + count > items_.size())
        return false;

    beginRemoveRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        items_.removeAt(row);  // assuming QVector<Character>
    }
    endRemoveRows();
    return true;
}

Character* CharacterModel::getItem(const QModelIndex &index) {
    if (!index.isValid() || index.row() < 0 || index.row() >= items_.size())
        return nullptr;

    return &items_[index.row()];
}

QVector<Character> CharacterModel::getItems() const {
    return items_;
}

void CharacterModel::setItem(const QModelIndex &index, const Character& character) {
    if (!index.isValid() || index.row() < 0 || index.row() >= items_.size()) return;
    items_[index.row()] = character;
    emit dataChanged(index, index);
}

void CharacterModel::appendItem(QString name, int initiative, Cyrus::CharacterType characterType){
    beginInsertRows(QModelIndex(), items_.size(), items_.size());
    items_.emplace_back(name, initiative, characterType);
    endInsertRows();
}

void CharacterModel::appendItem(Character rhs){
    beginInsertRows(QModelIndex(), items_.size(), items_.size());
    items_.push_back(std::move(rhs));
    endInsertRows();
}

void CharacterModel::addItems(const QVector<Character>& characters) {
    if (characters.isEmpty()) return;
    beginInsertRows(QModelIndex(), items_.size(), items_.size() + characters.size() - 1);
    items_.reserve(items_.size() + characters.size());
    for (const auto& c : characters) {
        items_.push_back(c);
    }
    endInsertRows();
}

void CharacterModel::advanceSegment(int segment) {
    if(segment_ > segment) advanceRound();
    segment_ = segment;
    emit segmentUpdate(segment_);

    if (!items_.isEmpty()) {
            emit dataChanged(index(0,0), index(items_.size()-1, 0), { CharacterRole, SegmentRole });
    }
}

void CharacterModel::backtrackSegment(int segment){
    if(segment_ < segment) backtrackRound();
    segment_ = segment;
    emit segmentUpdate(segment_);
    if (!items_.isEmpty()) {
            emit dataChanged(index(0,0), index(items_.size()-1, 0), { CharacterRole, SegmentRole });
    }
}

void CharacterModel::advanceRound() {
    ++round_;
    emit roundUpdate(round_);
}

void CharacterModel::backtrackRound() {
    if(round_ > 1) --round_;
    emit roundUpdate(round_);
}