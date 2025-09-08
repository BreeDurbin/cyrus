#include "CyrusDialog.h"
#include "ui_CyrusDialog.h"
#include "../style/StyleRepository.h"
#include <iostream>
#include <QStringList>
#include <QGroupBox>
#include <QTextEdit>
#include <QStringListModel>
#include <QHBoxLayout>
#include "../helper/TomlHelper.h"
#include "../helper/Enums.h"
#include <QVector>
#include <QShortcut>


CyrusDialog::CyrusDialog(QWidget *parent) 
    : QMainWindow(parent), 
      ui(new Ui::CyrusDialog)
{
    Q_INIT_RESOURCE(icon);
    Q_INIT_RESOURCE(image);

    setWindowIcon(IconRepository::miniCyrus());

    ui->setupUi(this);

    // Character models
    characterModel_ = new CharacterModel(this);
    initiativeModel_ = new CharacterModel(this);
    combatLogModel_ = new CharacterModel(this);

    // Attach models to views
    ui->characterRepository->setModel(characterModel_);
    ui->initiativeOrder->setModel(initiativeModel_);
    ui->combatLog->setModel(combatLogModel_);

    // Now it's safe to get the selection models
    characterSelection_ = ui->characterRepository->selectionModel();
    combatLogSelection_ = ui->combatLog->selectionModel();

    initiativeDelegate_ = new InitiativeDelegate(this);
    combatLogDelegate_ = new CombatLogDelegate(this);

    initiativeView_ = ui->initiativeOrder;

    //Configure styling
    setupHeroFrame();
    setupLabelStyleSheets();
    setupLineEditStyleSheets();
    setupSpinBoxStyleSheets();
    setupButtonStyleSheets();
    setupItemViews();
    setupConnections();

    //title and size
    setStyleSheet(StyleRepository::mainWindow());
    //setStyleSheet("#CyrusDialog { border-image: url(:/image/background4.png) 0 0 0 0 stretch fixed;}");

}

CyrusDialog::~CyrusDialog()
{
    Q_CLEANUP_RESOURCE(icon);
    Q_CLEANUP_RESOURCE(image);
    delete ui;
}

void CyrusDialog::setupHeroFrame() {
    QPixmap bg(":/image/CYRUS_HERO.png");
    ui->heroFrame->setFixedHeight(bg.height());  // lock vertical size
    ui->heroFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->heroFrame->setStyleSheet(StyleRepository::hero());
}

void CyrusDialog::setupLabelStyleSheets() {
    ui->participantsLabel->setStyleSheet(StyleRepository::label());
    ui->initiativeOrderLabel->setStyleSheet(StyleRepository::label());
    ui->combatLogLabel->setStyleSheet(StyleRepository::label());
    ui->segmentCounterLabel->setStyleSheet(StyleRepository::label());
    ui->roundCounterLabel->setStyleSheet(StyleRepository::label());
}

void CyrusDialog::setupLineEditStyleSheets(){
    ui->characterNameLineEdit->setStyleSheet(StyleRepository::lineEdit());
}

void CyrusDialog::setupSpinBoxStyleSheets(){
    ui->characterInitiativeSpinBox->setStyleSheet(StyleRepository::spinBox());
}

void CyrusDialog::setupButtonStyleSheets() {
    ui->addCharacterButton->setStyleSheet(StyleRepository::pushButton());
    ui->addToInitiativeButton->setStyleSheet(StyleRepository::pushButton());
    ui->addAllToInitiativeButton->setStyleSheet(StyleRepository::pushButton());
    ui->nextButton->setStyleSheet(StyleRepository::pushButton());
    ui->previousButton->setStyleSheet(StyleRepository::pushButton());

    // Next (Right Arrow)
    QAction* nextAction = new QAction(this);
    nextAction->setShortcut(QKeySequence(Qt::Key_Right));
    connect(nextAction, &QAction::triggered, ui->nextButton, &QPushButton::click);
    this->addAction(nextAction);

    // Previous (Left Arrow)
    QAction* prevAction = new QAction(this);
    prevAction->setShortcut(QKeySequence(Qt::Key_Left));
    connect(prevAction, &QAction::triggered, ui->previousButton, &QPushButton::click);
    this->addAction(prevAction);

    //scale up the icon a little bit, this will expand and resize the container
    int height = 1.15 * ui->saveButton->parentWidget()->height(); // same for both save and load icons

    QIcon saveIcon = IconRepository::save();
    ui->saveButton->setIcon(saveIcon);
    ui->saveButton->setFixedSize(height, height);
    ui->saveButton->setIconSize(QSize(height, height));
    ui->saveButton->setStyleSheet(StyleRepository::iconPushButton());

    QIcon loadIcon = IconRepository::load();
    ui->loadButton->setIcon(loadIcon);
    ui->loadButton->setFixedSize(height, height);
    ui->loadButton->setIconSize(QSize(height, height));
    ui->loadButton->setStyleSheet(StyleRepository::iconPushButton());

}

void CyrusDialog::setupItemViews(){
    // Character Repository
    ui->characterRepository->setStyleSheet(StyleRepository::itemView());
    ui->characterRepository->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->characterRepository->setFocusPolicy(Qt::NoFocus);
    ui->characterRepository->setMouseTracking(true);
    // Initiative Order
    ui->initiativeOrder->setStyleSheet(StyleRepository::itemView());
    ui->initiativeOrder->setSelectionMode(QAbstractItemView::NoSelection);
    ui->initiativeOrder->setFocusPolicy(Qt::NoFocus);
    ui->initiativeOrder->setItemDelegate(initiativeDelegate_);
    ui->initiativeOrder->setMouseTracking(true);
    // Combat Log
    ui->combatLog->setStyleSheet(StyleRepository::itemView());
    ui->combatLog->setSelectionMode(QAbstractItemView::NoSelection);
    ui->combatLog->setFocusPolicy(Qt::NoFocus);
    ui->combatLog->setItemDelegate(combatLogDelegate_);
    ui->combatLog->setMouseTracking(true);
}

void CyrusDialog::setupConnections(){

    //setup addCharacterButton to add an item to the characterRepository using the text in the characterNameLineEdit
    QObject::connect(
        ui->addCharacterButton, &QPushButton::clicked,
        [this](){
            QString text = ui->characterNameLineEdit->text();
            if(text.isEmpty()) return;
            int initiative = ui->characterInitiativeSpinBox->value();
            int classId = ui->iconSelector->currentIndex();
            Cyrus::CharacterType type = static_cast<Cyrus::CharacterType>(classId);
            characterModel_->appendItem(text, initiative, type);
            ui->characterNameLineEdit->clear(); // Clear the line edit after adding
            QModelIndex bottomIndex = characterModel_->index(characterModel_->rowCount() - 1, 0);
            characterSelection_->setCurrentIndex(bottomIndex, QItemSelectionModel::ClearAndSelect);
        }
    );

    //setup saveButton to save off toml file
    QObject::connect(
        ui->saveButton, &QPushButton::clicked,
        [this](){
            //Save off characters
            qDebug() << "Saving repository ...";
            const std::string filename = "default_name.toml";
            const auto & characters = characterModel_->getItems();
            if(characters.empty()) {
                qDebug() << "No characters to save. Saving failed.";
                return;
            }
            TomlHelper::saveRepository(filename, characterModel_->getItems());
        }
    );

    //setup loadButton to load in toml file
    QObject::connect(
        ui->loadButton, &QPushButton::clicked,
        [this](){
            //Load in characters
            qDebug() << "Loading repository ...";
            const std::string filename = "default_name.toml";
            const auto& characters = TomlHelper::loadRepository(filename);
            qDebug() << "Loaded " << characters.size() << "characters.";
            characterModel_->addItems(characters);
            // Auto select bottom most character (Most recent add)
            QModelIndex bottomIndex = characterModel_->index(characterModel_->rowCount() - 1, 0);
            characterSelection_->setCurrentIndex(bottomIndex, QItemSelectionModel::ClearAndSelect);
        }
    );

    //Setup characterRepository to add a character to the initiativeOrder
    QObject::connect(
        ui->addToInitiativeButton, &QPushButton::clicked,
        [this]() {
            QModelIndex index = characterSelection_->currentIndex();
            if (Character* character = characterModel_->getItem(index)) {
                initiativeModel_->appendItem(*character);
                initiativeModel_->sort(0);

                // If nothing in initiativeView is selected, select the first row
                if (!initiativeView_->hasActiveIndex() && initiativeModel_->rowCount() > 0) {
                    QModelIndex first = initiativeModel_->index(0, 0);
                    initiativeView_->setActiveIndex(first); 
                }
            }
        }
    );


    // Add all characters in repo to initiative order
    QObject::connect(
        ui->addAllToInitiativeButton, &QPushButton::clicked,
        [this]() {
            // Loop over all rows in characterModel_
            for (int row = 0; row < characterModel_->rowCount(); ++row) {
                QModelIndex idx = characterModel_->index(row, 0);
                if (Character* character = characterModel_->getItem(idx)) {
                    initiativeModel_->appendItem(*character);
                }
            }

            // Sort initiative after adding
            initiativeModel_->sort(0);

            // If nothing in initiativeOrder is selected, select the first row
            if (!initiativeView_->hasActiveIndex() && initiativeModel_->rowCount() > 0) {
                QModelIndex first = initiativeModel_->index(0, 0);
                initiativeView_->setActiveIndex(first);
            }
        }
    );

    //Setup initiativeOrder progress the order and add to the combat log
    QObject::connect(
        ui->nextButton, &QPushButton::clicked,
        [this](){
            QModelIndex index = initiativeView_->activeIndex();
            if (Character* character = initiativeModel_->getItem(index)) {
                combatLogModel_->appendItem(*character);
            }
            //Move selection for initiative selection to next
            selectNext(initiativeModel_);
            //keep combat log scrolled to bottom
            ui->combatLog->scrollToBottom();
        }
    );

    //Setup initiativeOrder to go back if the previous button is clicked
    QObject::connect(
        ui->previousButton, &QPushButton::clicked,
        [&](){
            selectPrevious(initiativeModel_);
        }
    );

    //connect delete buttons 
    connect(initiativeDelegate_, &InitiativeDelegate::deleteItem, this,
        [this](const QModelIndex &index) {
            initiativeModel_->removeRow(index.row());
        }
    );

    //connect segment counter and round counter
    connect(initiativeModel_, &CharacterModel::segmentUpdate, this,
        [this](int segment){
            QString str = QString("Segment: ")  + QString::number(segment) + QString(" / 10");
            ui->segmentCounterLabel->setText(str);
        }
    );

    connect(initiativeModel_, &CharacterModel::roundUpdate, this,
        [this](int round){
            QString str = QString("Round: ")  + QString::number(round);
            ui->roundCounterLabel->setText(str);
        }
    );

}


//Helpers
void CyrusDialog::selectNext(const QAbstractItemModel* model) {
    QModelIndex active = initiativeView_->hasActiveIndex()
        ? initiativeView_->activeIndex()
        : model->index(0, 0);

    int nextRow = (active.row() + 1) % model->rowCount();
    QModelIndex nextIndex = model->index(nextRow, 0);

    // keep initiativeOrder in sync
    Character c = model->data(nextIndex, CharacterModel::CharacterRole).value<Character>();
    initiativeModel_->advanceSegment(c.initiative());

    //selection model should be completely overriden and we use out own selection system based on segments and the underlying character model
    initiativeView_->setActiveIndex(nextIndex);
}

void CyrusDialog::selectPrevious(const QAbstractItemModel* model) {
    QModelIndex active = initiativeView_->hasActiveIndex()
        ? initiativeView_->activeIndex()
        : model->index(0, 0);

    int previousRow = active.row() - 1;
    if (previousRow < 0) {
        // Wrap around to the last rows
        previousRow = model->rowCount() - 1;
    }

    QModelIndex previousIndex = model->index(previousRow, 0);

    // keep initiativeOrder in sync
    Character c = model->data(previousIndex, CharacterModel::CharacterRole).value<Character>();
    initiativeModel_->backtrackSegment(c.initiative());

    //selection model should be completely overriden and we use out own selection system based on segments and the underlying character model
    initiativeView_->setActiveIndex(previousIndex);
}