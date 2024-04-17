#include<QStyledItemDelegate>

class CStyledItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:

    CStyledItemDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};
