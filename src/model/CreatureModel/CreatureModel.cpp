#include "CreatureModel.h"
#include <QBrush>
#include <QColor>
#include <QFont>
#include <QIcon>
#include "colorrepository.h"
#include "../Creature/Creature.h"


CreatureModel::CreatureModel(QObject *parent){
    auto temp1 = new Creature("Goblin1", Creature::Role::Monster, 11);
    auto temp2 = new Creature("Goblin2", Creature::Role::Monster, 10);
    auto temp3 = new Creature("Goblin3", Creature::Role::Monster, 14);
    auto temp4 = new Creature("Goblin4", Creature::Role::Monster, -2);
    auto temp5 = new Creature("PC1", Creature::Role::PC, 15);
    auto temp6 = new Creature("NPC1", Creature::Role::NPC, 305);
    creatures.push_back(*temp1);
    creatures.push_back(*temp2);
    creatures.push_back(*temp3);
    creatures.push_back(*temp4);
    creatures.push_back(*temp5);
    creatures.push_back(*temp6);
}

int CreatureModel::rowCount(const QModelIndex &parent) const{
    return !parent.isValid() ? creatures.size() : 0;
}

int CreatureModel::columnCount(const QModelIndex &parent) const{
    return !parent.isValid() ? 4 : 0;
}

QVariant CreatureModel::data(const QModelIndex &index, int role) const{
    if (!index.isValid()) return QVariant();

    switch(role){
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::FontRole:
            return QFont("Helvetica", 20, QFont::Bold);
        case Qt::BackgroundRole:
            return QBrush(ColorRepository::baseBackground());
        case Qt::ForegroundRole:    
            return QBrush(ColorRepository::text());
        case Qt::DecorationRole:
            if(index.column() == 0 && index.row() == currentRow)
                return QIcon(":/icon/initiative.png");
            else 
                return QVariant();
        case Qt::DisplayRole:
            if(index.column() == 0)
                return QString(creatures[index.row()].toString());
            else 
                return QVariant();
        case Qt::EditRole:
            if(index.column() == 0)
                return creatures[index.row()].toString();
            else 
                return QVariant();
        case Qt::SizeHintRole:
        return QSize(700, 50);
        default:
            return QVariant();
    }
}

QHash<int, QByteArray> CreatureModel::roleNames() const{
    QHash<int, QByteArray> roles;
    roles[Qt::TextAlignmentRole] = "textAlignment";
    roles[Qt::FontRole] = "font";
    roles[Qt::BackgroundRole] = "background";
    roles[Qt::ForegroundRole] = "foreground";
    roles[Qt::DecorationRole] = "decoration";
    roles[Qt::DisplayRole] = "display";
    roles[Qt::EditRole] = "edit";
    roles[Qt::SizeHintRole] = "sizeHint";
    return roles;
}

bool CreatureModel::setData(const QModelIndex &index, const QVariant &value, int role){
    if (index.isValid() && role == Qt::EditRole){
        creatures[index.row()] = value.value<Creature>();
        emit dataChanged(index, index, {role});
        return true;
    }
    return false;
}

Qt::ItemFlags CreatureModel::flags(const QModelIndex &index) const{
    return index.isValid() ? Qt::ItemIsEditable | QAbstractTableModel::flags(index) : Qt::NoItemFlags;
}

void CreatureModel::addRows(Creature &creature, const QModelIndex &parent){
    beginInsertRows(parent, rowCount(parent), rowCount(parent) - 1);
    creatures.push_back(creature);
    endInsertRows();
}

void CreatureModel::sort(){
    std::sort(creatures.begin(), creatures.end(), [](const Creature &a, const Creature &b){
        return a.initiative() > b.initiative();
    });
    emit dataChanged(index(0, 0), index(creatures.size() - 1, 0), {Qt::DisplayRole});
}

void CreatureModel::nextRow(){
    if(creatures.size() == 0) return;
    currentRow = (currentRow + 1) % creatures.size();
    if(currentRow == 0) emit newRound(++roundCounter);
    emit dataChanged(index(currentRow, 0), index(currentRow, 3), {Qt::DisplayRole});
}

void CreatureModel::newCombat(){
    creatures.clear();
    currentRow = 0;
    roundCounter = 0;
    emit newRound(roundCounter);
    emit dataChanged(index(currentRow, 0), index(currentRow, 3), {Qt::DisplayRole});
}

std::vector<Creature> CreatureModel::getCreatures(){
    return creatures;
}

void CreatureModel::addCreatures(std::vector<Creature> &creatures){
    this->creatures = creatures;
    emit dataChanged(index(0, 0), index(creatures.size() - 1, 3), {Qt::DisplayRole});
}