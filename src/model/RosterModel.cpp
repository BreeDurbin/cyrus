#include "RosterModel.h"
#include "Character.h"

#include <QDebug>

// Custom role for characters
namespace {
    constexpr int CharacterRole = Qt::UserRole + 1;
    constexpr int SentinelRole  = Qt::UserRole + 2;
}

RosterModel::RosterModel(QObject* parent)
    : QAbstractListModel(parent) {}


int RosterModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    // +1 sentinel row
    return items_.size() + 1;
}

QVariant RosterModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return {};

    const int row = index.row();

    // --- Sentinel row ---
    if (index.row() == items_.size()) {
        if (role == Qt::DisplayRole || role == Cyrus::CharacterRole)
            return QVariant::fromValue(Character::createSentinel());
        return {};
    }

    // --- Normal item ---
    if (row < 0 || row >= items_.size())
        return {};

    const auto &character = items_[row];

    if (role == Qt::DisplayRole) {
        return character ? character->name() : QString();
    }
    if (role == CharacterRole) {
        return QVariant::fromValue(character);
    }

    return {};
}

QHash<int, QByteArray> RosterModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "display";
    roles[CharacterRole]   = "character";
    roles[SentinelRole]    = "sentinel";
    return roles;
}

Qt::ItemFlags RosterModel::flags(const QModelIndex& index) const {
    if (!index.isValid())
        return Qt::NoItemFlags;

    if (index.row() == items_.size()) {
        // Sentinel row: editable for entering new text
        return Qt::ItemIsEnabled | Qt::ItemIsEditable;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool RosterModel::removeRows(int row, int count, const QModelIndex &parent) {
    if (parent.isValid())
        return false;
    if (row < 0 || row + count > items_.size())
        return false;

    beginRemoveRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i)
        items_.removeAt(row);
    endRemoveRows();
    return true;
}

// --- Accessors ---

std::shared_ptr<Character> RosterModel::getItem(const QModelIndex &index) {
    if (!index.isValid())
        return {};
    int row = index.row();
    if (row < 0 || row >= items_.size())
        return {};
    return items_[row];
}

QVector<std::shared_ptr<Character>> RosterModel::getItems(Cyrus::Faction faction) const {
    QVector<std::shared_ptr<Character>> result;
    for (const auto& c : items_) {
        if (c && c->faction() == faction)
            result.push_back(c);
    }
    return result;
}

QVector<std::shared_ptr<Character>> RosterModel::getItems() const {
    return items_;
}

void RosterModel::setItem(const QModelIndex &index, std::shared_ptr<Character> character) {
    if (!index.isValid())
        return;
    int row = index.row();
    if (row < 0 || row >= items_.size())
        return;

    items_[row] = character;
    emit dataChanged(index, index);
}

void RosterModel::appendItem(std::shared_ptr<Character> rhs) {
    const int row = items_.size();
    beginInsertRows(QModelIndex(), row, row);
    items_.append(rhs);
    endInsertRows();
}

void RosterModel::addItems(const QVector<std::shared_ptr<Character>>& characters) {
    if (characters.isEmpty())
        return;
    const int startRow = items_.size();
    const int endRow   = startRow + characters.size() - 1;
    beginInsertRows(QModelIndex(), startRow, endRow);
    items_ += characters;
    endInsertRows();
}

void RosterModel::removeItem(const QModelIndex& index) {
    if (!index.isValid())
        return;
    int row = index.row();
    if (row < 0 || row >= items_.size())
        return;
    removeRows(row, 1);
}

void RosterModel::clear() {
    if (items_.isEmpty())
        return;
    beginRemoveRows(QModelIndex(), 0, items_.size() - 1);
    items_.clear();
    endRemoveRows();
}
