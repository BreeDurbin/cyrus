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
#include "RosterLayoutEngine.h"
#include "CharacterLayoutEngine.h"
#include "CombatLogLayoutEngine.h"
#include "CastAction.h"
#include <filesystem>

#include "CyrusButton.h"
#include <QMessageBox>
#include <QScrollBar>


CyrusDialog::CyrusDialog(QWidget *parent) 
    : QMainWindow(parent), 
    ui(new Ui::CyrusDialog)
{
    Q_INIT_RESOURCE(control);
    Q_INIT_RESOURCE(icon);
    Q_INIT_RESOURCE(image);
    Q_INIT_RESOURCE(font);

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
    setupIconSelector();
    setupLabels();
    setupLineEdits();
    setupButtons();
    setupComboBoxes();
    setupCustomWidgets();
    setupItemViews();
    setupConnections();
    // important to be called last after all other ui configured
    setupMainDialog();
}

CyrusDialog::~CyrusDialog()
{
    Q_CLEANUP_RESOURCE(control);
    Q_CLEANUP_RESOURCE(icon);
    Q_CLEANUP_RESOURCE(image);
    Q_CLEANUP_RESOURCE(font);
    delete ui;
}

void CyrusDialog::setupMainDialog() {

    //setup QFrame widgets so they report minimum width:

    // Compute minimum width from all children inside initiativeFrame
    int minInitiativeWidth = ui->initiativeFrame->layout()->minimumSize().width();
    ui->initiativeFrame->setMinimumWidth(minInitiativeWidth);
    ui->initiativeFrame->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

    // HeroFrame
    int minHeroWidth = ui->heroFrame->layout()->minimumSize().width();
    ui->heroFrame->setMinimumWidth(minHeroWidth);
    ui->heroFrame->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

    // Then update central widget and main window
    qDebug() << "Setting main dialog width " << minInitiativeWidth + minHeroWidth + layout()->spacing();
    ui->centralwidget->setMinimumWidth(minInitiativeWidth + minHeroWidth + layout()->spacing());
    setMinimumSize(ui->centralwidget->minimumSize());


    setWindowIcon(IconRepository::miniCyrus_icon());
    setStyleSheet(StyleRepository::mainWindow());
    layout()->activate();

    qDebug() << "Dumping widget sizes: ";

    qDebug() << "centralwidget minWidth:" << ui->centralwidget->minimumWidth()
            << "layout min:" << layout()->minimumSize().width();

    auto l = ui->centralwidget->layout();
    if (!l) return;

    for (int i = 0; i < l->count(); ++i) {
        QLayoutItem* item = l->itemAt(i);
        QWidget* w = item->widget();
        if (!w) continue;

        QFrame* f = qobject_cast<QFrame*>(w);
        QAbstractItemView* view = qobject_cast<QAbstractItemView*>(w);

        int scrollbarWidth = view ? view->verticalScrollBar()->sizeHint().width() : 0;

        qDebug() << w->objectName()
                << "minWidth:" << w->minimumWidth()
                << "minSizeHint:" << w->minimumSizeHint().width()
                << "sizeHint:" << w->sizeHint().width()
                << "frameWidth:" << (f ? f->frameWidth() : 0)
                << "scrollbarWidth:" << scrollbarWidth
                << "actual:" << w->width();
    }

}

void CyrusDialog::setupHeroFrame() {
    QPixmap bg(":/image/CYRUS_HERO.png");
    ui->heroFrame->setFixedHeight(bg.height());  // lock vertical size
    ui->heroFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->heroFrame->setStyleSheet(StyleRepository::hero());
}

void CyrusDialog::setupIconSelector() {
    ui->iconSelector->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    ui->iconSelector->setMinimumWidth(ui->iconSelector->sizeHint().width());
    qDebug() << "IconSelector sizeHint:" << ui->iconSelector->sizeHint().width();
}

void CyrusDialog::setupLabels() {
    ui->participantsLabel->setStyleSheet(StyleRepository::label());
    ui->initiativeOrderLabel->setStyleSheet(StyleRepository::label());
    ui->combatLogLabel->setStyleSheet(StyleRepository::label());
    ui->roundCounterLabel->setStyleSheet(StyleRepository::label());
}

void CyrusDialog::setupLineEdits(){
    ui->characterNameLineEdit->setStyleSheet(StyleRepository::lineEdit());
}

void CyrusDialog::setupButtons() {

    //ui->addCharacterButton->setStyleSheet(StyleRepository::pushButton());
    ui->addToInitiativeButton->setStyleSheet(StyleRepository::pushButton());
    ui->addAllToInitiativeButton->setStyleSheet(StyleRepository::pushButton());

    // Clear buttons
    {
        int height = 1.15 * ui->initiativeOrderClearButton->parentWidget()->height();

        ui->initiativeOrderClearButton->setFixedSize(height, height);
        ui->initiativeOrderClearButton->setIconSize(QSize(height, height));
        ui->initiativeOrderClearButton->setStyleSheet(StyleRepository::iconPushButton());

        ui->combatLogClearButton->setFixedSize(height, height);
        ui->combatLogClearButton->setIconSize(QSize(height, height));
        ui->combatLogClearButton->setStyleSheet(StyleRepository::iconPushButton());
    }

    // Next / Prev buttons
    {
        int height = 1.15 * ui->nextButton->parentWidget()->height();

        QAction* nextAction = new QAction(this);
        nextAction->setShortcut(QKeySequence(Qt::Key_Right));
        connect(nextAction, &QAction::triggered, ui->nextButton, &QPushButton::click);
        this->addAction(nextAction);
        ui->nextButton->setVisible(false); // only visible in resolution step
        ui->nextButton->setStyleSheet(StyleRepository::iconPushButton());
        ui->nextButton->setFixedSize(height, height);
        ui->nextButton->setIconSize(QSize(height, height));

        QAction* prevAction = new QAction(this);
        prevAction->setShortcut(QKeySequence(Qt::Key_Left));
        connect(prevAction, &QAction::triggered, ui->previousButton, &QPushButton::click);
        this->addAction(prevAction);
        ui->previousButton->setVisible(false); // only visible in resolution step
        ui->previousButton->setStyleSheet(StyleRepository::iconPushButton());
        ui->previousButton->setFixedSize(height, height);
        ui->previousButton->setIconSize(QSize(height, height));
    }

    // Save / Load buttons
    {
        int height = 1.15 * ui->saveButton->parentWidget()->height();

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

void CyrusDialog::setupCustomWidgets(){
    //ui->combatSequenceStateStepper;
}

void CyrusDialog::setupItemViews(){

    auto dummy = std::make_shared<Character>("SIZE_HINT_DEFAULT", 10, 0, 0, Cyrus::CharacterType::Fighter, Cyrus::ActionType::Cast);
    auto castDummy = std::make_shared<CastAction>(*dummy, "ABI_DALZIM'S_HORRID_WILTING", 10, 0);

    // Roster
    {
        RosterLayoutEngine engine;
        const int rosterWidth = engine.minimumWidth(dummy);
        qDebug() << "Setting roster Item View width to: " << rosterWidth;
        ui->initiativeOrder->setMinimumWidth(rosterWidth);
        ui->roster->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

        ui->roster->setStyleSheet(StyleRepository::itemView());
        ui->roster->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->roster->setFocusPolicy(Qt::NoFocus);
        ui->roster->setItemDelegate(rosterDelegate_);
        ui->roster->setMouseTracking(true);
    }

    // Initiative Order
    {
        CharacterLayoutEngine engine;
        const int initiativeOrderWidth = engine.minimumWidth(dummy);
        qDebug() << "Setting initiativeOrder Item View width to: " << initiativeOrderWidth;
        ui->initiativeOrder->setMinimumWidth(initiativeOrderWidth);
        //ui->initiativeOrderFrame->setMinimumWidth(initiativeOrderWidth);
        ui->initiativeOrder->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

        ui->initiativeOrder->setStyleSheet(StyleRepository::itemView());
        ui->initiativeOrder->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->initiativeOrder->setFocusPolicy(Qt::NoFocus);
        ui->initiativeOrder->setItemDelegate(initiativeDelegate_);
        ui->initiativeOrder->setMouseTracking(true);
    }

    // Combat Log
    {
        CombatLogLayoutEngine engine;
        const int combatLogWidth = engine.minimumWidth(castDummy);
        qDebug() << "Setting combatLog Item View width to: " << combatLogWidth;
        ui->combatLog->setMinimumWidth(combatLogWidth);
        //ui->combatLogFrame->setMinimumWidth(combatLogWidth);
        ui->combatLog->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

        ui->combatLog->setStyleSheet(StyleRepository::itemView());
        ui->combatLog->setSelectionMode(QAbstractItemView::NoSelection);
        ui->combatLog->setFocusPolicy(Qt::NoFocus);
        ui->combatLog->setItemDelegate(combatLogDelegate_);
        ui->combatLog->setMouseTracking(true);
    }
}

void CyrusDialog::setupConnections(){

    // Toggle group initiative
    connect(ui->groupInitiativeCheckBox, &QCheckBox::toggled,
            characterController_, &CharacterController::setGroupInitiative);

    //setup addCharacterButton to add character to the roster
    connect(ui->addCharacterButton, &QPushButton::clicked,
        this, [this]() {
            QString text = ui->characterNameLineEdit->text();
            ui->characterNameLineEdit->clear();
            if (text.isEmpty()) return;
            int classId = ui->iconSelector->currentIndex();
            Cyrus::CharacterType type = static_cast<Cyrus::CharacterType>(classId);
            QModelIndex bottomIndex = characterController_->addToRoster(text, type);
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

    // load button
    connect(ui->loadButton, &QPushButton::clicked, this, [this]() {
        const QString selectedRoster = ui->rosterGroupsComboBox->currentText();
        if (selectedRoster.isEmpty()) {
            qWarning() << "No roster selected to load.";
            return;
        }

        if (characterController_->isRosterModified()) {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(
                this,
                "Unsaved Changes",
                "You have unsaved changes in your roster. Do you want to save before loading another roster?",
                QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel
            );

            if (reply == QMessageBox::Cancel) {
                return; // abort load
            } else if (reply == QMessageBox::Yes) {
                const auto current = characterController_->currentRosterName();
                if (!current.empty()) {
                    characterController_->saveRoster(current);
                } else {
                    // if no roster was loaded yet, fallback to combo box name
                    characterController_->saveRoster(selectedRoster.toStdString());
                }
            }
        }

        // Now safe to load the new roster
        QModelIndex bottomIndex = characterController_->loadRoster(selectedRoster.toStdString());
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

    // combat sequence state machine wiring
    connect(ui->combatSequenceStateStepper,       &PillStepper::stepForward,
            characterController_,                 &CharacterController::nextCombatSequenceState);

    connect(ui->combatSequenceStateStepper,       &PillStepper::stepBackward,
        characterController_,                     &CharacterController::previousCombatSequenceState);

    connect(characterController_, &CharacterController::combatSequenceStateChanged,
        ui->combatSequenceStateStepper, &PillStepper::updateCombatSequenceState);

    // only show next/prev buttons on resolution step
    connect(characterController_, &CharacterController::combatSequenceStateChanged,
        this, &CyrusDialog::updateCombatStepperButtons);
    
    // trigger repaint of initiative order on combat sequence change
    connect(characterController_, &CharacterController::combatSequenceStateChanged,
            ui->initiativeOrder->viewport(), QOverload<>::of(&QWidget::update));


    // next/prev Button wiring
    connect(ui->nextButton,       &QPushButton::clicked,
            characterController_, &CharacterController::advanceInitiative);
    connect(ui->previousButton,   &QPushButton::clicked,
            characterController_, &CharacterController::backtrackInitiative);

    // Center keep the current initiative scrolled to center
    connect(characterController_, &CharacterController::currentInitiativeChanged,
        this, &CyrusDialog::centerOnCurrentInitiative);

    // clear buttons
    connect(ui->initiativeOrderClearButton, &QPushButton::clicked,
            characterController_, &CharacterController::clearInitiativeOrder);
    connect(ui->combatLogClearButton, &QPushButton::clicked,
            characterController_, &CharacterController::clearCombatLog);


    //connect round counter
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

    // character
    connect(initiativeDelegate_, &InitiativeDelegate::deleteItem,
        characterController_, &CharacterController::deleteItem);
    
    connect(initiativeDelegate_,  QOverload<const QModelIndex&, Cyrus::ActionType>::of(&InitiativeDelegate::iconSelected),
            characterController_, QOverload<const QModelIndex&, Cyrus::ActionType>::of(&CharacterController::updateAction));
    
    connect(initiativeDelegate_, &InitiativeDelegate::cancelAction,
            characterController_, &CharacterController::cancelAction);
    
    connect(initiativeDelegate_, &InitiativeDelegate::decrementInitiative,
            characterController_, &CharacterController::decrementInitiative);

    connect(initiativeDelegate_, &InitiativeDelegate::incrementInitiative,
            characterController_, &CharacterController::incrementInitiative);

    //Cast actions (character dropdown)

    connect(initiativeDelegate_, &InitiativeDelegate::spellNameEdited,
        characterController_, &CharacterController::setSpellName);

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

    // Attack actions (character dropdown)
    connect(initiativeDelegate_, &InitiativeDelegate::decrementAttackAmount,
            characterController_, &CharacterController::decrementAttackAmount);

    connect(initiativeDelegate_, &InitiativeDelegate::incrementAttackAmount,
            characterController_, &CharacterController::incrementAttackAmount);

    connect(initiativeDelegate_, &InitiativeDelegate::decrementWeaponSpeed,
            characterController_, &CharacterController::decrementWeaponSpeed);

    connect(initiativeDelegate_, &InitiativeDelegate::incrementWeaponSpeed,
            characterController_, &CharacterController::incrementWeaponSpeed);

    connect(initiativeDelegate_, &InitiativeDelegate::attackSubmitted,
            characterController_, &CharacterController::attackSubmitted);

    // Misc actions 

    connect(initiativeDelegate_,   QOverload<const QModelIndex&, Cyrus::MiscActionType>::of(&InitiativeDelegate::iconSelected),
            characterController_,  QOverload<const QModelIndex&, Cyrus::MiscActionType>::of(&CharacterController::updateAction));

    connect(initiativeDelegate_, &InitiativeDelegate::miscSubmitted,
            characterController_, &CharacterController::miscSubmitted);

    //Connect signals for roster delegate to characterController_
    connect(rosterDelegate_,      &RosterDelegate::nextFaction,
            characterController_, &CharacterController::nextFaction);

    connect(rosterDelegate_,      &RosterDelegate::previousFaction,
            characterController_, &CharacterController::previousFaction);

    connect(rosterDelegate_,      &RosterDelegate::cloneRosterMember,
            characterController_, &CharacterController::cloneRosterMember);
            
    connect(rosterDelegate_,      &RosterDelegate::deleteRosterMember,
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

void CyrusDialog::updateCombatStepperButtons(Cyrus::CombatSequenceState state)
{
    bool showButtons = (state == Cyrus::CombatSequenceState::RESOLUTION);
    ui->nextButton->setVisible(showButtons);
    ui->previousButton->setVisible(showButtons);
}

void CyrusDialog::centerOnCurrentInitiative(const QModelIndex& index) {
    if (index.isValid()) {
        ui->initiativeOrder->scrollTo(index, QAbstractItemView::PositionAtCenter);
    }
}
