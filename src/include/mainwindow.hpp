#pragma once

#include <QMainWindow>
#include <QVBoxLayout>

namespace Ui {
class MainWindow;
}

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

protected:
    Ui::MainWindow *ui;

};
