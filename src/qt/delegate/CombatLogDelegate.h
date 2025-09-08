#pragma once
#include <QStyledItemDelegate>
#include <QModelIndex>

class CombatLogDelegate : public QStyledItemDelegate
{
    Q_OBJECT
    public:
        explicit CombatLogDelegate(QObject *parent = nullptr);
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
        QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};
