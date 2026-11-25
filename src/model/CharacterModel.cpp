#include "CharacterModel.h"
#include "Character.h"
#include <QColor>
#include <QFont>
#include <QIcon>
#include <QAbstractItemView>
#include "Enums.h"
#include "ColorRepository.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <numeric>

using namespace Cyrus;

CharacterModel::CharacterModel(QObject* parent) 
    : QAbstractListModel(parent) { }

int CharacterModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 0;
    return items_.size();
}

QVariant CharacterModel::data(const QModelIndex &index, int role) const {
    if (index.row() < 0 || index.row() >= items_.size()) return QVariant();
    if (index.column() != 0) return QVariant();

    auto item = items_.at(index.row());

    switch (role) {
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::FontRole:
            return QFont("Helvetica", 14, QFont::Bold);
        case Qt::ForegroundRole:
            return QBrush(ColorRepository::text());
        case Qt::DecorationRole:
            return QIcon(":/icon/initiative.png");
        case Qt::DisplayRole:
            return item->name();
        case Qt::EditRole:
            return QVariant::fromValue(QString());
        case Qt::SizeHintRole:
            return QSize(80, 50);
        case Cyrus::ModelRoles::CharacterRole:
            return QVariant::fromValue(item);
        case Cyrus::ModelRoles::InitiativeRole:
            return QVariant::fromValue(initiative_);
        case Cyrus::ModelRoles::RoleTypeRole: {
            auto item = items_.at(index.row());
            // ensure it's really a base Character, not some derived Action type
            if (typeid(*item) == typeid(Character)) {
                if (item->actionType() == Cyrus::ActionType::Attack) {
                    return QVariant::fromValue(
                        static_cast<int>(Cyrus::ModelRoles::ActionNameEditRole)
                    );
                }
                if (item->actionType() == Cyrus::ActionType::Cast) {
                    return QVariant::fromValue(
                        static_cast<int>(Cyrus::ModelRoles::SpellNameEditRole)
                    );
                }
            }
            return QVariant();
        }
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
    roles[Cyrus::ModelRoles::CharacterRole]      = "character";
    roles[Cyrus::ModelRoles::InitiativeRole]     = "initiative";
    roles[Cyrus::ModelRoles::RoleTypeRole]       = "roletyperole";
    roles[Cyrus::ModelRoles::SpellNameEditRole]  = "spellnameeditrole";
    roles[Cyrus::ModelRoles::ActionNameEditRole] = "actionnameeditrole";

    return roles;
}

Qt::ItemFlags CharacterModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) return Qt::NoItemFlags;

    Qt::ItemFlags f = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    // Add editable flag for spell name rows
    if (index.data(Cyrus::ModelRoles::RoleTypeRole).toInt() == Cyrus::SpellNameEditRole) {
        f |= Qt::ItemIsEditable;
    }

    return f;
}

void CharacterModel::sort(int row, Cyrus::CombatSequenceState state) {
    if (row < 0 || row >= items_.size()) return;

    using Item = std::shared_ptr<Character>;
    std::function<bool(const Item&, const Item&)> lessThan;

    switch (state) {
    case Cyrus::CombatSequenceState::PLAYER_DETERMINATION:
        // Blue faction first, then initiative
        lessThan = [](const Item& lhs, const Item& rhs) {
            bool lhsBlue = lhs->faction() == Cyrus::Faction::Blue;
            bool rhsBlue = rhs->faction() == Cyrus::Faction::Blue;
            if (lhsBlue != rhsBlue) return lhsBlue;
            return lhs->initiative() < rhs->initiative();
        };
        break;

    case Cyrus::CombatSequenceState::NPC_DETERMINATION:
        // Non-Blue faction first, then initiative
        lessThan = [](const Item& lhs, const Item& rhs) {
            bool lhsBlue = lhs->faction() == Cyrus::Faction::Blue;
            bool rhsBlue = rhs->faction() == Cyrus::Faction::Blue;
            if (lhsBlue != rhsBlue) return !lhsBlue;
            return lhs->initiative() < rhs->initiative();
        };
        break;

    case Cyrus::CombatSequenceState::INITIATIVE:
        // Characters (base class) before derived types, then initiative
        lessThan = [](const Item& lhs, const Item& rhs) {
            bool lhsIsCharacter = typeid(*lhs) == typeid(Character);
            bool rhsIsCharacter = typeid(*rhs) == typeid(Character);
            if (lhsIsCharacter != rhsIsCharacter)
                return lhsIsCharacter;
            return lhs->initiative() < rhs->initiative();
        };
        break;

    case Cyrus::CombatSequenceState::RESOLUTION:
        // Initiative only
        lessThan = [](const Item& lhs, const Item& rhs) {
            return lhs->initiative() < rhs->initiative();
        };
        break;
    }

    emit layoutAboutToBeChanged({}, QAbstractItemModel::VerticalSortHint);

    // Sort row numbers rather than items so every old row maps to exactly one
    // new row, even if the same character appears twice. Stable so ties keep
    // their current order instead of shuffling on every re-sort.
    QVector<int> order(items_.size() - row);
    std::iota(order.begin(), order.end(), row);
    std::stable_sort(order.begin(), order.end(), [&](int lhs, int rhs) {
        return lessThan(items_[lhs], items_[rhs]);
    });

    QVector<int> newRowOf(items_.size());
    std::iota(newRowOf.begin(), newRowOf.end(), 0);
    QVector<Item> sorted = items_;
    for (int i = 0; i < order.size(); ++i) {
        sorted[row + i] = items_[order[i]];
        newRowOf[order[i]] = row + i;
    }
    items_ = std::move(sorted);

    // Keep persistent indexes (e.g. InitiativeView's active row) on their character.
    const QModelIndexList oldIndexes = persistentIndexList();
    QModelIndexList newIndexes;
    newIndexes.reserve(oldIndexes.size());
    for (const QModelIndex& old : oldIndexes) {
        newIndexes << index(newRowOf.value(old.row(), old.row()), old.column());
    }
    changePersistentIndexList(oldIndexes, newIndexes);

    emit layoutChanged({}, QAbstractItemModel::VerticalSortHint);
}

QModelIndex CharacterModel::indexFromCharacter(const std::shared_ptr<Character>& character) const {
    const int row = items_.indexOf(character);
    if (row < 0)
        return QModelIndex(); // invalid if not found
    return index(row, 0, QModelIndex());
}

bool CharacterModel::removeRows(int row, int count, const QModelIndex &parent) {
    if (row < 0 || row + count > items_.size())
        return false;

    beginRemoveRows(parent, row, row + count - 1);
    items_.remove(row, count);
    endRemoveRows();
    return true;
}

std::shared_ptr<Character> CharacterModel::getItem(const QModelIndex &index) {
    if (!index.isValid() || index.row() < 0 || index.row() >= items_.size())
        return nullptr;
    return items_[index.row()];
}

QVector<std::shared_ptr<Character>> CharacterModel::getItems() const {
    return items_;
}

QVector<std::shared_ptr<Character>> CharacterModel::getItems(Cyrus::Faction faction) const {
    QVector<std::shared_ptr<Character>> result;
    for (const auto& c : items_) {
        if (c->faction() == faction)
            result.push_back(c);
    }
    return result;
}

void CharacterModel::setItem(const QModelIndex &index, std::shared_ptr<Character> character) {
    if (!index.isValid() || index.row() < 0 || index.row() >= items_.size()) return;

    items_[index.row()] = character;

    emit dataChanged(index, index);
}

void CharacterModel::appendItem(std::shared_ptr<Character> rhs){
    int row = items_.size();
    beginInsertRows(QModelIndex(), row, row);
    items_.push_back(rhs);
    endInsertRows();
}

void CharacterModel::addItems(const QVector<std::shared_ptr<Character>>& characters) {
    if (characters.isEmpty()) return;

    int startRow = items_.size();
    int endRow   = startRow + characters.size() - 1;

    beginInsertRows(QModelIndex(), startRow, endRow);
    items_ += characters;
    endInsertRows();
}

void CharacterModel::removeItem(const QModelIndex& index) {
    if (!index.isValid())
        return;
    removeRows(index.row(), 1);
}


void CharacterModel::clear() {
    beginResetModel();
    items_.clear();
    initiative_ = 0;
    round_ = 1;
    endResetModel();
    emit roundUpdate(round_);
}

void CharacterModel::advanceInitiative(int initiative) {
    if(initiative_ > initiative){
        //new round so update all characters as not having acted
        for(auto character : items_){
            character->setActed(false);
        }
        qDebug() << "Advancing round";
        advanceRound();
    }
    
    initiative_ = initiative;

    if (!items_.isEmpty()) {
        emit dataChanged(index(0,0), index(items_.size()-1, 0), { CharacterRole });
    }
}

void CharacterModel::backtrackInitiative(int initiative){
    if(initiative_ < initiative) backtrackRound();
    initiative_ = initiative;
    if (!items_.isEmpty()) {
        emit dataChanged(index(0,0), index(items_.size()-1, 0), { CharacterRole });
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
