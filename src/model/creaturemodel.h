#include <QAbstractItemModel>

class CreatureModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    CreatureModel(QObject *parent = nullptr) : QAbstractItemModel(parent);
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex());
    QModelIndex parent(const QModelIndex &child);
    int rowCount(const QModelIndex &parent = QModelIndex());
    int columnCount(const QModelIndex &parent = QModelIndex());
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole);
};