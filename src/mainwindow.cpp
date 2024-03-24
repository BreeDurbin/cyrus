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
    this->setWindowTitle("Cyrus: Combat Tracker");
    this->setMinimumSize(1920, 1080);

    QHBoxLayout *outer = new QHBoxLayout(this);

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
    inner->addWidget(new QLabel("Initiative Tracker"));

    QListView *initiativeList = new QListView();
    initiativeList->setModel(new QStringListModel(QStringList() << "Billiam" << "Taliesin" << "Egon" << "Edra"));
    inner->addWidget(initiativeList);
    initiativeList->show();

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
    auto pcList = new QStringListModel(QStringList() << "Billiam" << "Taliesin" << "Egon" << "Edra");
    pcList->setHeaderData(0, Qt::Horizontal, "Characters");
    pcList->setHeaderData(1, Qt::Horizontal, "Initiative");
    pcList->setHeaderData(2, Qt::Horizontal, "HP");
    pcRepository->setModel(pcList);
    pcRepository->setSelectionMode(QAbstractItemView::SingleSelection);
    pcRepository->setSelectionBehavior(QAbstractItemView::SelectRows);
    pcRepository->setEditTriggers(QAbstractItemView::NoEditTriggers);
    pcRepository->setDragEnabled(true);
    inner->addWidget(pcRepository);
    pcRepository->show();

    // Effect Tracker
    QHBoxLayout *effectBannerLayout = new QHBoxLayout;
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
    effectTracker->show();
}
