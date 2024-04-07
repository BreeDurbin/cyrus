#include "include/creaturemodel.hpp"
#include <QBrush>
#include <QColor>
#include <QFont>
#include <QIcon>


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

QVariant CreatureModel::data(const QModelIndex &index, int role) const{
    if (!index.isValid())
        return QVariant();

    switch(role){
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::FontRole:
            return QFont("Helvetica", 16, QFont::Bold);
        case Qt::BackgroundRole:
            return QBrush(QColor(60, 60, 60));
        case Qt::ForegroundRole:    
            return QBrush(QColor(255, 255, 255));
        case Qt::DecorationRole:
            return QIcon(":/icons/monster.png");
        case Qt::DisplayRole:
            return QString(creatures[index.row()].name + " " + creatures[index.row()].type + " " + QString::number(creatures[index.row()].level) + " " + QString::number(creatures[index.row()].initiative));
        case Qt::EditRole:
            return QString(creatures[index.row()].name + " " + creatures[index.row()].type + " " + QString::number(creatures[index.row()].level) + " " + QString::number(creatures[index.row()].initiative));
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