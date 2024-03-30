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

    setStyleSheet("#MainWindow { border-image: url(:/images/cyrus_background.png) 0 0 0 0 stretch stretch;}");

    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    //setBackgroundRole(QPalette::Background);

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
    //persian gold 235, 180, 15
    // deep gold 194, 155, 12
    QVBoxLayout *inner = new QVBoxLayout;
    outer->addLayout(inner);
    auto banner = new QLabel("Initiative Tracker");
    banner->setFont(QFont("Helvetica", 24, QFont::Bold));
    auto palette = banner->palette();
    palette.setColor(banner->foregroundRole(), QColor(235, 180, 15));
    banner->setPalette(palette);

    inner->addWidget(banner);

    QListView *initiativeRepository = new QListView();
    initiativeRepository->viewport()->setAutoFillBackground(false);
    auto initiativeList = new CreatureModel();
    initiativeRepository->setModel(initiativeList);
    inner->addWidget(initiativeRepository);
    initiativeRepository->show();

    QHBoxLayout *innerBottom = new QHBoxLayout;
    inner->addLayout(innerBottom);

    QPushButton *sortButton = new QPushButton("Sort");
    sortButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    sortButton->setFixedWidth(100);
    sortButton->setFixedHeight(50);
    sortButton->setFont(QFont("Helvetica", 16, QFont::Bold));
    innerBottom->addWidget(sortButton);

    QPushButton *nextButton = new QPushButton("Next");
    nextButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    nextButton->setFixedWidth(100);
    nextButton->setFixedHeight(50);
    nextButton->setFont(QFont("Helvetica", 16, QFont::Bold));

QPalette pal = nextButton->palette();
    pal.setColor(QPalette::Button, QColor(12, 51, 194));
    pal.setColor(QPalette::ButtonText, QColor(235, 180, 15));
    pal.setColor(QPalette::Highlight, QColor(235, 180, 15));
    nextButton->setAutoFillBackground(true);
    nextButton->setPalette(pal);
    nextButton->update();

    QObject::connect(sortButton, &QPushButton::clicked, initiativeList, &CreatureModel::sort);

    innerBottom->addWidget(nextButton);
}


void MainWindow::initializeCreatureRepository(QHBoxLayout* outer){
    
    QVBoxLayout *inner = new QVBoxLayout;
    outer->addLayout(inner);

    // Player Character Repository
    QHBoxLayout *creatureRepositoryBannerLayout = new QHBoxLayout;
    inner->addLayout(creatureRepositoryBannerLayout);
    
    auto banner = new QLabel("Character Repository");
    banner->setFont(QFont("Helvetica", 24, QFont::Bold));
    auto palette = banner->palette();
    palette.setColor(banner->foregroundRole(), QColor(235, 180, 15));
    banner->setPalette(palette);

    creatureRepositoryBannerLayout->addWidget(banner);
    QPushButton *addPcButton = new QPushButton("Add Player Character");
    creatureRepositoryBannerLayout->addWidget(addPcButton);

    QListView *creatureRepository = new QListView();
    creatureRepository->viewport()->setAutoFillBackground(false);
    auto pcList = new CreatureModel();
    creatureRepository->setModel(pcList);
    inner->addWidget(creatureRepository);
    creatureRepository->show();

    //connections
    connect(addPcButton, &QPushButton::clicked, [pcList](){
        CreatureItem temp;
        temp.name = "New PC";
        temp.type = "PC";
        temp.level = 1;
        temp.initiative = 0;
        pcList->addRows(temp);
    });

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
