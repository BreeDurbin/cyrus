#include "mainwindow.h"

#include <QStringList>
#include "creaturecombobox.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    centralWidget = new QWidget();
    setCentralWidget(centralWidget);

    QVBoxLayout *layout = new QVBoxLayout;
    centralWidget->setLayout(layout);

    initializeInitiative(layout);
    initializeCreatureRepository(layout);


    centralWidget->show();
}

MainWindow::~MainWindow()
{
}

void MainWindow::initializeInitiative(QVBoxLayout* layout){

    std::cout << "initializeInitiative" << std::endl;


    QPushButton *button1 = new QPushButton("One");
    QPushButton *button2 = new QPushButton("Two");
    QPushButton *button3 = new QPushButton("Three");
    QPushButton *button4 = new QPushButton("Four");
    QPushButton *button5 = new QPushButton("Five");

    layout->addWidget(button1);
    layout->addWidget(button2);
    layout->addWidget(button3);
    layout->addWidget(button4);
    layout->addWidget(button5);

}


void MainWindow::initializeCreatureRepository(QVBoxLayout* layout){

    std::cout << "initializeCreatureRepository1" << std::endl;

    QComboBox *pcComboBox = new CreatureComboBox();

    pcComboBox->addItems({ "Billiam", "Taliesin", "Egon", "Edra" });

    QComboBox *monsterComboBox = new CreatureComboBox();

    std::cout << "initializeCreatureRepository2" << std::endl;

    layout->addWidget(pcComboBox);
    layout->addWidget(monsterComboBox);

    std::cout << "initializeCreatureRepository3" << std::endl;
}