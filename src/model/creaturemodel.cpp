#include "creaturemodel.h"
#include <QBrush>
#include <QColor>
#include <QFont>
#include <QIcon>


CreatureModel::CreatureModel(QObject *parent){
    auto temp = new CreatureItem;
    temp->name = "root";
    temp->type = "root";
    temp->level = 0;
    temp->initiative = 0;
    creatures.push_back(*temp);
    auto temp2 = new CreatureItem;
    temp->name = "hiya";
    temp->type = "dwagon";
    temp->level = 9001;
    temp->initiative = 293;
    creatures.push_back(*temp2);
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

