#include "CyrusDialog.h"
#include "ui_CyrusDialog.h"


#include "StyleRepository.h"
#include <iostream>
#include <QStringList>
#include <QGroupBox>
#include <QTextEdit>
#include <QStringListModel>
#include <QHBoxLayout>
#include "Enums.h"
#include <QVector>
#include <QShortcut>
#include "CharacterLayoutEngine.h"
#include "CombatLogLayoutEngine.h"
#include "CastAction.h"
#include <filesystem>


CyrusDialog::CyrusDialog(QWidget *parent) 
    : QMainWindow(parent), 
    ui(new Ui::CyrusDialog)
{
    Q_INIT_RESOURCE(icon);
    Q_INIT_RESOURCE(image);

    ui->setupUi(this);

    //Wire up Controller
    characterController_ = new CharacterController(
        ui->initiativeOrder,
        //Models are parented by CyrusDialog so it will handle cleanup
        new CharacterModel(this),
        new CharacterModel(this),
        new CharacterModel(this),
        this
    );

    // Attach models to views
    ui->roster->setModel(characterController_->rosterModel());
    ui->initiativeOrder->setModel(characterController_->initiativeModel());
    ui->combatLog->setModel(characterController_->combatLogModel());

    // Now it's safe to get the selection models
    characterSelection_ = ui->roster->selectionModel();
    combatLogSelection_ = ui->combatLog->selectionModel();
    
    rosterDelegate_ = new RosterDelegate(characterController_, this);
    initiativeDelegate_ = new InitiativeDelegate(characterController_, this);
    combatLogDelegate_ = new CombatLogDelegate(this);

    //Configure styling
    setupHeroFrame();
    setupLabels();
    setupLineEdits();
    setupSpinBoxes();
    setupButtons();
    setupComboBoxes();
    setupItemViews();
    setupConnections();
    // important to be called last after all other ui configured
    setupMainDialog();
}

CyrusDialog::~CyrusDialog()
{
    Q_CLEANUP_RESOURCE(icon);
    Q_CLEANUP_RESOURCE(image);
    delete ui;
}

void CyrusDialog::setupMainDialog() {

    setWindowIcon(IconRepository::miniCyrus_icon());
    setStyleSheet(StyleRepository::mainWindow());
    //setStyleSheet("#CyrusDialog { border-image: url(:/image/background4.png) 0 0 0 0 stretch fixed;}");
    layout()->activate();
    int minW = ui->roster->minimumWidth()
             + ui->initiativeOrder->minimumWidth()
             + ui->combatLog->minimumWidth()
             + ui->iconSelector->minimumWidth()
             + layout()->spacing() * 3
             + layout()->contentsMargins().left()
             + layout()->contentsMargins().right();

    qDebug() << "Setting main dialog width " << minW;
    setMinimumWidth(minW);
}

void CyrusDialog::setupHeroFrame() {
    QPixmap bg(":/image/CYRUS_HERO.png");
    ui->heroFrame->setFixedHeight(bg.height());  // lock vertical size
    ui->heroFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->heroFrame->setStyleSheet(StyleRepository::hero());
}

void CyrusDialog::setupLabels() {
    ui->participantsLabel->setStyleSheet(StyleRepository::label());
    ui->initiativeOrderLabel->setStyleSheet(StyleRepository::label());
    ui->combatLogLabel->setStyleSheet(StyleRepository::label());
    ui->segmentCounterLabel->setStyleSheet(StyleRepository::label());
    ui->roundCounterLabel->setStyleSheet(StyleRepository::label());
}

void CyrusDialog::setupLineEdits(){
    ui->characterNameLineEdit->setStyleSheet(StyleRepository::lineEdit());
}

void CyrusDialog::setupSpinBoxes(){
    ui->characterInitiativeSpinBox->setStyleSheet(StyleRepository::spinBox());
}

void CyrusDialog::setupButtons() {
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

    QIcon saveIcon = IconRepository::save_icon();
    ui->saveButton->setIcon(saveIcon);
    ui->saveButton->setFixedSize(height, height);
    ui->saveButton->setIconSize(QSize(height, height));
    ui->saveButton->setStyleSheet(StyleRepository::iconPushButton());

    QIcon loadIcon = IconRepository::load_icon();
    ui->loadButton->setIcon(loadIcon);
    ui->loadButton->setFixedSize(height, height);
    ui->loadButton->setIconSize(QSize(height, height));
    ui->loadButton->setStyleSheet(StyleRepository::iconPushButton());

}

void CyrusDialog::setupComboBoxes() {
    // Apply style
    ui->rosterGroupsComboBox->setStyleSheet(StyleRepository::comboBox());
    ui->rosterGroupsComboBox->clear();
    ui->rosterGroupsComboBox->setEditable(true);

    // Populate from helper
    const QStringList rosterFiles = listFilenames("cyrus_data/roster");
    ui->rosterGroupsComboBox->addItems(rosterFiles);

    if (!rosterFiles.isEmpty()) {
        ui->rosterGroupsComboBox->setCurrentIndex(0);
    }
}

void CyrusDialog::setupItemViews(){

    // Roster
    ui->roster->setStyleSheet(StyleRepository::itemView());
    ui->roster->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->roster->setFocusPolicy(Qt::NoFocus);
    ui->roster->setItemDelegate(rosterDelegate_);
    ui->roster->setMouseTracking(true);

    // Initiative Order
    auto dummy      = std::make_shared<Character>("SIZE_HINT_DEFAULT", 10, Cyrus::CharacterType::Fighter, Cyrus::ActionType::Cast);
    CharacterLayoutEngine engine;
    const int initiativeOrderWidth = engine.minimumWidth(dummy);

    qDebug() << "Setting initiativeOrder Item View width to: " << initiativeOrderWidth;
    ui->initiativeOrder->setMinimumWidth(initiativeOrderWidth);
    // set min width for entire column
    ui->initiativeOrderFrame->setMinimumWidth(initiativeOrderWidth);

    ui->initiativeOrder->setStyleSheet(StyleRepository::itemView());
    ui->initiativeOrder->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->initiativeOrder->setFocusPolicy(Qt::NoFocus);
    ui->initiativeOrder->setItemDelegate(initiativeDelegate_);
    ui->initiativeOrder->setMouseTracking(true);
    // Combat Log
    dummy = std::make_shared<CastAction>("SIZE_HINT_DEFAULT", 10, Cyrus::CharacterType::Fighter, "ABI_DALZIM'S_HORRID_WILTING", 10, 0);
    
    const int combatLogWidth = CombatLogLayoutEngine::minimumWidth(dummy);
    qDebug() << "Setting combatLog Item View width to: " << combatLogWidth;
    ui->combatLog->setMinimumWidth(combatLogWidth);
    // set min width for entire column
    ui->combatLogFrame->setMinimumWidth(combatLogWidth);

    ui->combatLog->setStyleSheet(StyleRepository::itemView());
    ui->combatLog->setSelectionMode(QAbstractItemView::NoSelection);
    ui->combatLog->setFocusPolicy(Qt::NoFocus);
    ui->combatLog->setItemDelegate(combatLogDelegate_);
    ui->combatLog->setMouseTracking(true);
}

void CyrusDialog::setupConnections(){

    //setup addCharacterButton to add character to the roster
    connect(ui->addCharacterButton, &QPushButton::clicked,
        this, [this]() {
            QString text = ui->characterNameLineEdit->text();
            ui->characterNameLineEdit->clear();
            if (text.isEmpty()) return;
            int initiative = ui->characterInitiativeSpinBox->value();
            int classId = ui->iconSelector->currentIndex();
            Cyrus::CharacterType type = static_cast<Cyrus::CharacterType>(classId);
            QModelIndex bottomIndex = characterController_->addToRoster(text, initiative, type);
            characterSelection_->setCurrentIndex(bottomIndex, QItemSelectionModel::ClearAndSelect);
        }
    );

    //setup save/load to toml file

    //save button
    connect(ui->saveButton, &QPushButton::clicked, this, [this]() {
        const QString rosterName = ui->rosterGroupsComboBox->currentText().trimmed();
        if (rosterName.isEmpty()) {
            qWarning() << "No roster name entered to save.";
            return;
        }

        characterController_->saveRoster(rosterName.toStdString());
    });

    //load button
    connect(ui->loadButton, &QPushButton::clicked, this, [this]() {
        // Get the currently selected roster file from the combo box
        const QString selectedRoster = ui->rosterGroupsComboBox->currentText();
        if (selectedRoster.isEmpty()) {
            qWarning() << "No roster selected to load.";
            return;
        }

        // Load the selected roster into the controller
        QModelIndex bottomIndex = characterController_->loadRoster(selectedRoster.toStdString());

        // Update selection to the last row (bottomIndex) if needed
        if (bottomIndex.isValid() && characterSelection_) {
            characterSelection_->setCurrentIndex(bottomIndex, QItemSelectionModel::ClearAndSelect);
        }
    });

    // Connect combo box editing finished (when user presses Enter)
    connect(ui->rosterGroupsComboBox->lineEdit(), &QLineEdit::returnPressed, this, [this]() {
        const QString rosterName = ui->rosterGroupsComboBox->currentText().trimmed();
        if (rosterName.isEmpty()) {
            qWarning() << "Roster name is empty.";
            return;
        }

        QModelIndex bottomIndex = characterController_->loadRoster(rosterName.toStdString());

        if (bottomIndex.isValid() && characterSelection_) {
            characterSelection_->setCurrentIndex(bottomIndex, QItemSelectionModel::ClearAndSelect);
        }
    });

    //Add the selected character in the roster to the initiativeOrder
    connect(ui->addToInitiativeButton, &QPushButton::clicked,
            this, [this]() {
                const QModelIndex& index = characterSelection_->currentIndex();
                characterController_->addRosterMemberToInitiative(index);
            });

    // Add all characters in roster to initiative order
    QObject::connect(ui->addAllToInitiativeButton, &QPushButton::clicked,
                 characterController_, &CharacterController::addRosterToInitiative);

    // next/prev Button wiring
    connect(ui->nextButton,       &QPushButton::clicked,
            characterController_, &CharacterController::advanceInitiative);
    connect(ui->previousButton,   &QPushButton::clicked,
            characterController_, &CharacterController::backtrackInitiative);

    // clear buttons
    connect(ui->initiativeOrderClearButton, &QPushButton::clicked,
            characterController_, &CharacterController::clearInitiativeOrder);
    connect(ui->combatLogClearButton, &QPushButton::clicked,
            characterController_, &CharacterController::clearCombatLog);


    //connect segment counter and round counter
    connect(characterController_, &CharacterController::segmentChangedText,
            ui->segmentCounterLabel, &QLabel::setText);

    connect(characterController_, &CharacterController::roundChangedText,
            ui->roundCounterLabel, &QLabel::setText);

    // Have controller scroll combat dialog to bottom 
    connect(characterController_, &CharacterController::requestScrollCombatLogToBottom,
            ui->combatLog, [this]() {
                auto model = ui->combatLog->model();
                if (!model || model->rowCount() == 0) return;

                QModelIndex lastIndex = model->index(model->rowCount() - 1, 0);
                ui->combatLog->scrollTo(lastIndex, QAbstractItemView::PositionAtBottom);
            });


    // Connect signals from initiative delegate to characterController_

    //Cast actions
    connect(initiativeDelegate_, &InitiativeDelegate::deleteItem,
        characterController_, &CharacterController::deleteItem);
    
    connect(initiativeDelegate_, &InitiativeDelegate::spellNameEdited,
        characterController_, &CharacterController::setSpellName);

    connect(initiativeDelegate_, &InitiativeDelegate::iconSelected,
            characterController_, &CharacterController::updateAction);

    connect(initiativeDelegate_, &InitiativeDelegate::decrementCastingTime,
            characterController_, &CharacterController::decrementCastingTime);

    connect(initiativeDelegate_, &InitiativeDelegate::incrementCastingTime,
            characterController_, &CharacterController::incrementCastingTime);

    connect(initiativeDelegate_, &InitiativeDelegate::decrementDuration,
            characterController_, &CharacterController::decrementDuration);

    connect(initiativeDelegate_, &InitiativeDelegate::incrementDuration,
            characterController_, &CharacterController::incrementDuration);

    connect(initiativeDelegate_, &InitiativeDelegate::castSubmitted,
            characterController_, &CharacterController::castSubmitted);

    // Attack actions
    connect(initiativeDelegate_, &InitiativeDelegate::decrementAttackAmount,
            characterController_, &CharacterController::decrementAttackAmount);

    connect(initiativeDelegate_, &InitiativeDelegate::incrementAttackAmount,
            characterController_, &CharacterController::incrementAttackAmount);

    connect(initiativeDelegate_, &InitiativeDelegate::attackSubmitted,
            characterController_, &CharacterController::attackSubmitted);

    //Connect signals for roster delegate to characterController_
    connect(rosterDelegate_,      &RosterDelegate::incrementRosterMemberInitiativeClicked,
            characterController_, &CharacterController::incrementRosterMemberInitiative);

    connect(rosterDelegate_,      &RosterDelegate::decrementRosterMemberInitiativeClicked,
            characterController_, &CharacterController::decrementRosterMemberInitiative);

    connect(rosterDelegate_,      &RosterDelegate::cloneRosterMemberClicked,
            characterController_, &CharacterController::cloneRosterMember);
            
    connect(rosterDelegate_,      &RosterDelegate::deleteButtonClicked,
            characterController_, &CharacterController::deleteRosterMember);

}

QStringList CyrusDialog::listFilenames(const std::string& directoryPath) {
    QStringList result;
    const std::filesystem::path dirPath{directoryPath};

    if (!std::filesystem::exists(dirPath) || !std::filesystem::is_directory(dirPath)) {
        qWarning() << "Directory does not exist:" << directoryPath;
        return result;
    }

    try {
        for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".toml") {
                result.append(QString::fromStdString(entry.path().stem().string()));
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        qWarning() << "Error reading directory:" << e.what();
    }

    return result;
}


