#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class CreaturePage; }
QT_END_NAMESPACE

class CreaturePage : public QWidget
{
    Q_OBJECT

public:
    explicit CreaturePage(QWidget *parent = nullptr);
    ~CreaturePage();

private:
    Ui::CreaturePage *ui;
};
