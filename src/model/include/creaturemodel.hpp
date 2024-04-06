#include <QAbstractListModel>


struct CreatureItem // nested class
{
    QString name;
    QString type;
    int level;
    int initiative;
};

class CreatureModel : public QAbstractListModel{

    Q_OBJECT

public:
    CreatureModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    void addRows(CreatureItem &item, const QModelIndex &parent = QModelIndex());
    void sort();


private:
    std::vector<CreatureItem> creatures;
};