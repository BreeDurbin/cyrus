#include "mainwindow.h"

#include <QStringList>
#include <iostream>
#include <QGroupBox>
#include <QLabel>
#include <QTextEdit>
#include <QListView>
#include <QStringListModel>
#include "model/creaturemodel.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    //title and size
    this->setWindowTitle("Cyrus: Combat Tracker");
    this->setMinimumSize(1920, 1080);
    this->setObjectName("MainWindow");

    setStyleSheet("#MainWindow { background-image: url(:/images/cyrus_background.png); }");

    // core layout
    QHBoxLayout *outer = new QHBoxLayout();
    QWidget *center = new QWidget();
    setCentralWidget(center);
    center->setLayout(outer);

    // initialization calls
    initializeInitiative(outer);
    initializeCreatureRepository(outer);

    // final show call
    this->centralWidget()->show();
}

MainWindow::~MainWindow()
{
}

void MainWindow::initializeInitiative(QHBoxLayout* outer){

    QVBoxLayout *inner = new QVBoxLayout;
    outer->addLayout(inner);
    inner->addWidget(new QLabel("Initiative Tracker"));

    QListView *initiativeRepository = new QListView();
    auto initiativeList = new CreatureModel();
    initiativeRepository->setModel(initiativeList);
    inner->addWidget(initiativeRepository);
    initiativeRepository->show();

    QHBoxLayout *innerBottom = new QHBoxLayout;
    inner->addLayout(innerBottom);

    QPushButton *sortButton = new QPushButton("Sort");
    innerBottom->addWidget(sortButton);

    QPushButton *nextButton = new QPushButton("Next");
    innerBottom->addWidget(nextButton);
}


void MainWindow::initializeCreatureRepository(QHBoxLayout* outer){
    
    QVBoxLayout *inner = new QVBoxLayout;
    outer->addLayout(inner);

    // Player Character Repository
    QHBoxLayout *pcRepositoryBannerLayout = new QHBoxLayout;
    inner->addLayout(pcRepositoryBannerLayout);
    pcRepositoryBannerLayout->addWidget(new QLabel("Character Repository"));
    QPushButton *addPcButton = new QPushButton("Add Player Character");
    pcRepositoryBannerLayout->addWidget(addPcButton);

    QListView *pcRepository = new QListView();
    auto pcList = new CreatureModel();
    pcRepository->setModel(pcList);
    inner->addWidget(pcRepository);
    pcRepository->show();

    // Effect Tracker
    /*QHBoxLayout *effectBannerLayout = new QHBoxLayout;
    inner->addLayout(effectBannerLayout);
    effectBannerLayout->addWidget(new QLabel("Effect Tracker"));
    QPushButton *addEffectButton = new QPushButton("Add Effect");
    effectBannerLayout->addWidget(addEffectButton);

    QListView *effectTracker = new QListView();
    auto effectList = new QStringListModel(QStringList() << "Cloudkill" << "Stoneskin" << "Stunned");
    effectList->setHeaderData(0, Qt::Horizontal, "Target");
    effectList->setHeaderData(1, Qt::Horizontal, "Duration Left");
    effectTracker->setModel(effectList);
    effectTracker->setDragEnabled(true);
    inner->addWidget(effectTracker);
    effectTracker->show();*/
}
