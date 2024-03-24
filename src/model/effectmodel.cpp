#include <effectmodel.h>

EffectModel::EffectModel(QObject *parent) : QAbstractItemModel(parent)
{
    rootItem = new EffectItem();
}

EffectModel::~EffectModel()
{
    delete rootItem;
}

QModelIndex EffectModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    EffectItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<EffectItem *>(parent.internalPointer());

    EffectItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex EffectModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();

    EffectItem *childItem = static_cast<EffectItem *>(child.internalPointer());
    EffectItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int EffectModel::rowCount(const QModelIndex &parent) const
{
    EffectItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<EffectItem *>(parent.internalPointer());

    return parentItem->childCount();
}

int EffectModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant EffectModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    EffectItem *item = static_cast<EffectItem *>(index.internalPointer());

    return item->data(index.column());
}
