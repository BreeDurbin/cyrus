#include <QAbstractTableModel>

class Creature;

class CreatureModel : public QAbstractTableModel{

    Q_OBJECT

public:
    CreatureModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    void addRows(Creature &item, const QModelIndex &parent = QModelIndex());
    void sort();
    void nextRow();
    std::vector<Creature> getCreatures();
    void addCreatures(std::vector<Creature> &creatures);

    signals:
    void newRound(int roundCounter);

    public slots:
    void newCombat();


private:
    int currentRow;
    int roundCounter;
    std::vector<Creature> creatures;
};