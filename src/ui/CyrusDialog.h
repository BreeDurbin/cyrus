#pragma once

#include <QMainWindow>
#include <QItemSelectionModel>
#include <QAbstractItemModel>
#include "CharacterModel.h"
#include "InitiativeDelegate.h"
#include "CombatLogDelegate.h"
#include "InitiativeView.h"

QT_BEGIN_NAMESPACE
namespace Ui { class CyrusDialog; }
QT_END_NAMESPACE


class CyrusDialog : public QMainWindow {
    Q_OBJECT

public:
    explicit CyrusDialog(QWidget *parent = nullptr);
    ~CyrusDialog();

signals:
    void addRosterToInitiative();
    void addRosterMemberToInitiative(const QModelIndex& index);

private:
    Ui::CyrusDialog* ui;  // generated UI object

    // Delegates (UI only, draw/edit logic)
    InitiativeDelegate* initiativeDelegate_;
    CombatLogDelegate* combatLogDelegate_;

    // Selection models (tied to views, not controller)
    QItemSelectionModel* characterSelection_;
    QItemSelectionModel* combatLogSelection_;

    // Controller mediates between models and views
    CharacterController* characterController_;

    // UI-only setup helpers
    void setupMainDialog();
    void setupHeroFrame();
    void setupLabels();
    void setupLineEdits();
    void setupSpinBoxes();
    void setupButtons();
    void setupComboBoxes();
    void setupItemViews();
    void setupConnections();

    QStringList listFilenames(const std::string& directoryPath);

};
