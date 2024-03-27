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

private:
    std::vector<CreatureItem> creatures;
};