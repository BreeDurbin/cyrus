#ifndef EFFECTREPOSITORY_H
#define EFFECTREPOSITORY_H

#include <QListView>

class EffectRepository : public QListView
{
    Q_OBJECT

public:
    explicit EffectRepository(QWidget *parent = nullptr);
    ~EffectRepository();

    // Add any additional member functions or variables here

private:
    // Add any private member functions or variables here

};

#endif // EFFECTREPOSITORY_H