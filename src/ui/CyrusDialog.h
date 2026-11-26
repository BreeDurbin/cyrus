#pragma once

#include <QMainWindow>
#include <QItemSelectionModel>
#include <QAbstractItemModel>
#include "RosterDelegate.h"
#include "InitiativeDelegate.h"
#include "CombatLogDelegate.h"

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

public slots:
    void onRosterSelectionChanged(const QString& selectedRoster);
    // Shows the roster name in the roster combo box and loads it.
    void selectRoster(const QString& rosterName);

public:
    // Grabs the window once per tab, with that tab showing, in tab order. Each
    // grab is keyed by the tab's title in lower case (e.g. "combat"). The
    // current tab is restored afterwards.
    QList<QPair<QString, QPixmap>> grabTabs();

private:
    Ui::CyrusDialog* ui;  // generated UI object

    // Delegates (UI only, draw/edit logic)
    RosterDelegate* rosterDelegate_;
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
    void setupTabs();
    void setupLabels();
    void setupButtons();
    void setupComboBoxes();
    void setupCustomWidgets();
    void setupItemViews();
    void setupConnections();

    QStringList listFilenames(const std::string& directoryPath);
    void updateCombatStepperButtons(Cyrus::CombatSequenceState state);
    void centerOnCurrentInitiative(const QModelIndex& index);

};
