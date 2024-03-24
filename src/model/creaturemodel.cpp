#include "creaturemodel.h"

CreatureModel::CreatureModel(QObject *parent) : QAbstractItemModel(parent)
{
    rootItem = new CreatureItem();
}

CreatureModel::~CreatureModel()
{
    delete rootItem;
}

QModelIndex CreatureModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    CreatureItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<CreatureItem *>(parent.internalPointer());

    CreatureItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex CreatureModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();

    CreatureItem *childItem = static_cast<CreatureItem *>(child.internalPointer());
    CreatureItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int CreatureModel::rowCount(const QModelIndex &parent) const
{
    CreatureItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<CreatureItem *>(parent.internalPointer());

    return parentItem->childCount();
}

int CreatureModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<CreatureItem *>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

QVariant CreatureModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    CreatureItem *item = static_cast<CreatureItem *>(index.internalPointer());

    return item->data(index.column());
}


