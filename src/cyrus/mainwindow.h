#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QPushButton>
#include "widget/monstercombobox.h"
#include "widget/pccombobox.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QWidget *centralWidget;
    QPushButton* button;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void initializeInitiative(QVBoxLayout* layout);
    void initializeCreatureRepository(QVBoxLayout* layout);
};

#endif // MAINWINDOW_H
