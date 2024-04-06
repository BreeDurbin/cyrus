#pragma once

#include <QMainWindow>
#include <QVBoxLayout>

class MainWindow 
    : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void initializeInitiative(QHBoxLayout* layout);
    void initializeCreatureRepository(QHBoxLayout* layout);
};
