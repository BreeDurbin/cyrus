#include "CreatureModel.h"
#include <QBrush>
#include <QColor>
#include <QFont>
#include <QIcon>
#include "colorrepository.h"


CreatureModel::CreatureModel(QObject *parent){
    auto temp0 = new CreatureItem;
    temp0->name = "root0";
    temp0->type = "root0";
    temp0->level = 0;
    temp0->initiative = 3;
    creatures.push_back(*temp0);
    auto temp1 = new CreatureItem;
    temp1->name = "root1";
    temp1->type = "root1";
    temp1->level = 0;
    temp1->initiative = 32;
    creatures.push_back(*temp1);
    auto temp2 = new CreatureItem;
    temp2->name = "root2";
    temp2->type = "root2";
    temp2->level = 0;
    temp2->initiative = 0;
    creatures.push_back(*temp2);
    auto temp3 = new CreatureItem;
    temp3->name = "hiya";
    temp3->type = "dwagon";
    temp3->level = 9001;
    temp3->initiative = 81;
    creatures.push_back(*temp3);
}

int CreatureModel::rowCount(const QModelIndex &parent) const{
    return !parent.isValid() ? creatures.size() : 0;
}

int CreatureModel::columnCount(const QModelIndex &parent) const{
    return !parent.isValid() ? 4 : 0;
}

QVariant CreatureModel::data(const QModelIndex &index, int role) const{
    if (!index.isValid())
        return QVariant();

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
                return QString(creatures[index.row()].name + " " + creatures[index.row()].type + " " + QString::number(creatures[index.row()].level) + " " + QString::number(creatures[index.row()].initiative));
            else 
                return QVariant();
        case Qt::EditRole:
            if(index.column() == 0)
                return QString(creatures[index.row()].name + " " + creatures[index.row()].type + " " + QString::number(creatures[index.row()].level) + " " + QString::number(creatures[index.row()].initiative));
            else 
                return QVariant();
        case Qt::SizeHintRole:
            return QSize(1300, 50);
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
        auto temp = value.toString().split(" ");
        creatures[index.row()].name = temp[0];
        creatures[index.row()].type = temp[1];
        creatures[index.row()].level = temp[2].toInt();
        creatures[index.row()].initiative = temp[3].toInt();
        emit dataChanged(index, index, {role});
        return true;
    }
    return false;
}

Qt::ItemFlags CreatureModel::flags(const QModelIndex &index) const{
    return index.isValid() ? Qt::ItemIsEditable | QAbstractListModel::flags(index) : Qt::NoItemFlags;
}

void CreatureModel::addRows(CreatureItem &creatureItem, const QModelIndex &parent){
    beginInsertRows(parent, rowCount(parent), rowCount(parent) - 1);
    creatures.push_back(creatureItem);
    endInsertRows();
}

void CreatureModel::sort(){
    std::sort(creatures.begin(), creatures.end(), [](const CreatureItem &a, const CreatureItem &b){
        return a.initiative > b.initiative;
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

std::vector<CreatureItem> CreatureModel::getCreatures(){
    return creatures;
}

void CreatureModel::addCreatures(std::vector<CreatureItem> &creatures){
    this->creatures = creatures;
    emit dataChanged(index(0, 0), index(creatures.size() - 1, 3), {Qt::DisplayRole});
}