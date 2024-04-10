#include "include/cstyleditemdelegate.hpp"


CStyledItemDelegate::CStyledItemDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

void CStyledItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);
}

QSize CStyledItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QStyledItemDelegate::sizeHint(option, index);
}