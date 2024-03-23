#include "mainwindow.h"

#include <QStringList>
#include "creaturecombobox.h"
#include <iostream>
#include <QGroupBox>
#include <QLabel>
#include <QTextEdit>
#include <QListView>
#include <QStringListModel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setWindowTitle("Hello World");
    this->setMinimumSize(1920, 1080);

    QHBoxLayout *outer = new QHBoxLayout(this);
    auto title = new QLabel("Combat Tracker");

    QWidget *center = new QWidget();
    setCentralWidget(center);
    center->setLayout(outer);

    initializeInitiative(outer);
    initializeCreatureRepository(outer);

    this->centralWidget()->show();
}

MainWindow::~MainWindow()
{
}

void MainWindow::initializeInitiative(QHBoxLayout* outer){

    QVBoxLayout *inner = new QVBoxLayout;
    outer->addLayout(inner);

    QListView *initiativeList = new QListView();
    initiativeList->setModel(new QStringListModel(QStringList() << "Billiam" << "Taliesin" << "Egon" << "Edra"));
    inner->addWidget(initiativeList);
    initiativeList->show();
}


void MainWindow::initializeCreatureRepository(QHBoxLayout* outer){
    
    QVBoxLayout *inner = new QVBoxLayout;
    outer->addLayout(inner);

    QListView *pcRepository = new QListView();
    pcRepository->setModel(new QStringListModel(QStringList() << "Billiam" << "Taliesin" << "Egon" << "Edra"));
    inner->addWidget(pcRepository);
    pcRepository->show();


    QListView *monsterRepository = new QListView();
    monsterRepository->setModel(new QStringListModel(QStringList() << "Goblin" << "Orc" << "Giant" << "Dragon"));
    inner->addWidget(monsterRepository);
    monsterRepository->show();
}