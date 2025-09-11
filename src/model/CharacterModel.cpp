#include "CharacterModel.h"
#include "Character.h"
#include <QColor>
#include <QFont>
#include <QIcon>
#include <QAbstractItemView>
#include "Enums.h"
#include "ColorRepository.h"

#include <iostream>
#include <memory>

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
        case Cyrus::ModelRoles::SegmentRole:
            return QVariant::fromValue(segment_);
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
    roles[Cyrus::ModelRoles::SegmentRole]        = "segment";
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

void CharacterModel::sort(int /*column*/, Qt::SortOrder order) {
    if(order == Qt::AscendingOrder){
        std::sort(items_.begin(), items_.end(),
            [](const std::shared_ptr<Character>& lhs, const std::shared_ptr<Character>& rhs){
                return lhs->initiative() < rhs->initiative();
            });
    } else {
        std::sort(items_.begin(), items_.end(),
            [](const std::shared_ptr<Character>& lhs, const std::shared_ptr<Character>& rhs){
                return lhs->initiative() > rhs->initiative();
            });
    }
    emit dataChanged(index(0, 0), index(items_.size() - 1, 0), {Qt::DisplayRole});
}

bool CharacterModel::removeRows(int row, int count, const QModelIndex &parent) {
    if (row < 0 || row + count > items_.size())
        return false;

    beginRemoveRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        items_.removeAt(row);
    }
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

void CharacterModel::setItem(const QModelIndex &index, std::shared_ptr<Character> character) {
    if (!index.isValid() || index.row() < 0 || index.row() >= items_.size()) return;
    items_[index.row()] = std::move(character); // replace smart pointer
    emit dataChanged(index, index);
}


void CharacterModel::appendItem(QString name, int initiative, Cyrus::CharacterType characterType){
    beginInsertRows(QModelIndex(), items_.size(), items_.size());
    items_.push_back(std::make_shared<Character>(name, initiative, characterType));
    endInsertRows();
}

void CharacterModel::appendItem(std::shared_ptr<Character> rhs){
    beginInsertRows(QModelIndex(), items_.size(), items_.size());
    items_.push_back(rhs);
    endInsertRows();
}

void CharacterModel::addItems(const QVector<std::shared_ptr<Character>>& characters) {
    if (characters.isEmpty()) return;
    beginInsertRows(QModelIndex(), items_.size(), items_.size() + characters.size() - 1);
    items_.reserve(items_.size() + characters.size());
    for (const auto& c : characters) {
        items_.push_back(c);
    }
    endInsertRows();
}

void CharacterModel::removeItem(const QModelIndex& index) {
    if (!index.isValid())
        return;

    int row = index.row();
    if (row < 0 || row >= items_.size())
        return;

    beginRemoveRows(QModelIndex(), row, row);
    items_.removeAt(row);
    endRemoveRows();
}

void CharacterModel::clear() {
    beginResetModel();
    items_.clear();
    endResetModel();
}

void CharacterModel::advanceSegment(int segment) {
    if(segment_ > segment) advanceRound();
    segment_ = segment;
    emit segmentUpdate(segment_);

    if (!items_.isEmpty()) {
        emit dataChanged(index(0,0), index(items_.size()-1, 0),
                         { CharacterRole, SegmentRole });
    }
}

void CharacterModel::backtrackSegment(int segment){
    if(segment_ < segment) backtrackRound();
    segment_ = segment;
    emit segmentUpdate(segment_);
    if (!items_.isEmpty()) {
        emit dataChanged(index(0,0), index(items_.size()-1, 0),
                         { CharacterRole, SegmentRole });
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
