#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QPushButton>


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void initializeInitiative(QHBoxLayout* layout);
    void initializeCreatureRepository(QHBoxLayout* layout);
};

#endif // MAINWINDOW_H
