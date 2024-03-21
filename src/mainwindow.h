#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QPushButton>
#include "creaturecombobox.h"


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
