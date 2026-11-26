#include "TestHelpers.h"

#include <QAbstractItemView>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QTabWidget>

#include "CyrusComboBox.h"
#include "CyrusDialog.h"
#include "PillStepper.h"

using namespace TestHelpers;

namespace {

// Drives the real main window on the offscreen platform. Widgets are looked up by
// their objectName from CyrusDialog.ui, so the tests click the same buttons a user
// would and go through the same signal wiring.
struct DialogFixture {
    // Declared first so the dialog is built inside the temp dir and sees its rosters.
    ScopedTempWorkingDir dir;
    bool rostersCopied = copyGoldRosters();
    CyrusDialog dialog;

    bool copyGoldRosters() {
        const auto rosterDir = dir.path() / "cyrus_data" / "roster";
        std::filesystem::create_directories(rosterDir);
        for (const auto& entry : std::filesystem::directory_iterator(CYRUS_GOLD_DATA_DIR)) {
            std::filesystem::copy_file(entry.path(), rosterDir / entry.path().filename());
        }
        return true;
    }

    template <typename T>
    T* find(const char* name) {
        T* widget = dialog.findChild<T*>(QString::fromLatin1(name));
        REQUIRE(widget != nullptr);
        return widget;
    }

    QAbstractItemModel* model(const char* viewName) {
        return find<QAbstractItemView>(viewName)->model();
    }

    // The roster always ends with the inline "add" sentinel row.
    QStringList rosterNames() {
        QAbstractItemModel* roster = model("roster");
        QStringList names;
        for (int row = 0; row < roster->rowCount() - 1; ++row) {
            names << roster->index(row, 0).data().toString();
        }
        return names;
    }

    QStringList rosterMenuEntries() {
        QStringList entries;
        for (QAction* action : find<CyrusComboBox>("rosterGroupsComboBox")->findChild<QMenu*>()->actions()) {
            entries << action->text();
        }
        entries.sort();
        return entries;
    }

    void chooseRosterFromMenu(const QString& name) {
        for (QAction* action : find<CyrusComboBox>("rosterGroupsComboBox")->findChild<QMenu*>()->actions()) {
            if (action->text() == name) {
                action->trigger();
                return;
            }
        }
        FAIL("No roster menu entry named " << name);
    }

    void click(const char* buttonName) { find<QPushButton>(buttonName)->click(); }
    void stepForward(int times = 1) {
        for (int i = 0; i < times; ++i) emit find<PillStepper>("combatSequenceStateStepper")->stepForward();
    }
};

const QStringList kHerosParty{"Naofumi", "Raphtalia", "Filo-chan"};

}

TEST_CASE_METHOD(DialogFixture, "Dialog starts on the combat tab with an empty roster", "[ui]") {
    CHECK(find<QTabWidget>("mainTabWidget")->currentWidget()->objectName() == "combatTab");
    CHECK(rosterNames().isEmpty());
    CHECK(model("initiativeOrder")->rowCount() == 0);
    CHECK(model("combatLog")->rowCount() == 0);
    CHECK(find<PillStepper>("combatSequenceStateStepper")->text() == "NPC Determination");
    CHECK(find<QPushButton>("nextButton")->isHidden());
    CHECK(find<QPushButton>("previousButton")->isHidden());
}

TEST_CASE_METHOD(DialogFixture, "grabTabs grabs every tab by name and keeps the current tab", "[ui]") {
    dialog.show();
    const auto grabs = dialog.grabTabs();
    REQUIRE(grabs.size() == 2);
    CHECK(grabs[0].first == "combat");
    CHECK(grabs[1].first == "creatures");
    CHECK(grabs[0].second.size() == dialog.size() * dialog.devicePixelRatio());
    CHECK(grabs[0].second.toImage() != grabs[1].second.toImage());
    CHECK(find<QTabWidget>("mainTabWidget")->currentWidget()->objectName() == "combatTab");
}

TEST_CASE_METHOD(DialogFixture, "Roster menu lists the saved rosters in cyrus_data/roster", "[ui][roster]") {
    CHECK(rosterMenuEntries() == QStringList{"Band of the Hawk", "Encounter 1", "Hero's Party", "default_name"});
}

TEST_CASE("Roster menu is empty when there is no cyrus_data directory", "[ui][roster]") {
    ScopedTempWorkingDir dir;
    CyrusDialog dialog;
    CHECK(dialog.findChild<CyrusComboBox*>("rosterGroupsComboBox")->findChild<QMenu*>()->actions().isEmpty());
}

TEST_CASE_METHOD(DialogFixture, "Choosing a roster from the menu loads its members", "[ui][roster]") {
    chooseRosterFromMenu("Hero's Party");
    CHECK(rosterNames() == kHerosParty);
    CHECK(find<CyrusComboBox>("rosterGroupsComboBox")->currentText() == "Hero's Party");
}

TEST_CASE_METHOD(DialogFixture, "Typing a roster name and pressing Enter loads it", "[ui][roster]") {
    QLineEdit* edit = find<CyrusComboBox>("rosterGroupsComboBox")->lineEdit();
    edit->setText("Hero's Party");
    emit edit->returnPressed();
    CHECK(rosterNames() == kHerosParty);
}

TEST_CASE_METHOD(DialogFixture, "selectRoster loads a roster and shows its name", "[ui][roster]") {
    dialog.selectRoster("Hero's Party");
    CHECK(rosterNames() == kHerosParty);
    CHECK(find<CyrusComboBox>("rosterGroupsComboBox")->currentText() == "Hero's Party");
}

TEST_CASE_METHOD(DialogFixture, "Selecting a roster that doesn't exist leaves the roster empty", "[ui][roster]") {
    dialog.selectRoster("No Such Roster");
    CHECK(rosterNames().isEmpty());
}

TEST_CASE_METHOD(DialogFixture, "Save writes the roster file and adds it to the menu", "[ui][roster]") {
    dialog.selectRoster("Hero's Party");
    find<CyrusComboBox>("rosterGroupsComboBox")->setText("Saved Party");
    click("saveButton");

    CHECK(std::filesystem::exists(dir.path() / "cyrus_data" / "roster" / "Saved Party.toml"));
    CHECK(rosterMenuEntries().contains("Saved Party"));

    // Reload it from disk to check the contents round-trip.
    dialog.selectRoster("Saved Party");
    CHECK(rosterNames() == kHerosParty);
}

TEST_CASE_METHOD(DialogFixture, "Saving an empty roster writes nothing", "[ui][roster]") {
    find<CyrusComboBox>("rosterGroupsComboBox")->setText("Empty Party");
    click("saveButton");
    CHECK_FALSE(std::filesystem::exists(dir.path() / "cyrus_data" / "roster" / "Empty Party.toml"));
}

TEST_CASE_METHOD(DialogFixture, "Add all to initiative puts every roster member in the order", "[ui][initiative]") {
    dialog.selectRoster("Hero's Party");
    click("addAllToInitiativeButton");
    CHECK(model("initiativeOrder")->rowCount() == 3);
}

TEST_CASE_METHOD(DialogFixture, "Add to initiative adds only the selected roster member", "[ui][initiative]") {
    dialog.selectRoster("Hero's Party");
    QAbstractItemView* roster = find<QAbstractItemView>("roster");
    roster->selectionModel()->setCurrentIndex(roster->model()->index(1, 0), QItemSelectionModel::ClearAndSelect);
    click("addToInitiativeButton");

    QAbstractItemModel* initiative = model("initiativeOrder");
    REQUIRE(initiative->rowCount() == 1);
    CHECK(initiative->index(0, 0).data().toString() == "Raphtalia");
}

TEST_CASE_METHOD(DialogFixture, "The stepper walks the combat sequence", "[ui][sequence]") {
    PillStepper* stepper = find<PillStepper>("combatSequenceStateStepper");
    stepForward();
    CHECK(stepper->text() == "Player Determination");
    stepForward();
    CHECK(stepper->text() == "Initiative");
    stepForward();
    CHECK(stepper->text() == "Resolution");
    stepForward();
    CHECK(stepper->text() == "NPC Determination");

    emit stepper->stepBackward();
    CHECK(stepper->text() == "Resolution");
}

TEST_CASE_METHOD(DialogFixture, "Next and previous buttons only show in the resolution step", "[ui][sequence]") {
    QPushButton* next = find<QPushButton>("nextButton");
    QPushButton* previous = find<QPushButton>("previousButton");

    stepForward(2);
    CHECK(next->isHidden());
    CHECK(previous->isHidden());

    stepForward();
    CHECK_FALSE(next->isHidden());
    CHECK_FALSE(previous->isHidden());

    stepForward();
    CHECK(next->isHidden());
    CHECK(previous->isHidden());
}

TEST_CASE_METHOD(DialogFixture, "Next logs each turn and keeps everyone in the order", "[ui][initiative]") {
    dialog.selectRoster("Hero's Party");
    click("addAllToInitiativeButton");
    stepForward(3);

    for (int turn = 1; turn <= 4; ++turn) {
        click("nextButton");
        CHECK(model("combatLog")->rowCount() == turn);
    }
    CHECK(model("initiativeOrder")->rowCount() == 3);
}

// Regression: a new round used to start only when the next initiative was lower
// than the current one, so a freshly loaded roster (everyone on one initiative)
// stayed on round 1 forever.
TEST_CASE_METHOD(DialogFixture, "Wrapping past the last row counts a new round when initiatives are equal", "[ui][initiative]") {
    dialog.selectRoster("Hero's Party");
    click("addAllToInitiativeButton");
    stepForward(3);

    for (int turn = 0; turn < 3; ++turn) click("nextButton");
    CHECK(find<QLabel>("roundCounterLabel")->text() == "Round: 2");
}

TEST_CASE_METHOD(DialogFixture, "Clear buttons empty the initiative order and combat log", "[ui][initiative]") {
    dialog.selectRoster("Hero's Party");
    click("addAllToInitiativeButton");
    stepForward(3);
    click("nextButton");
    REQUIRE(model("combatLog")->rowCount() == 1);

    click("initiativeOrderClearButton");
    CHECK(model("initiativeOrder")->rowCount() == 0);
    CHECK(model("combatLog")->rowCount() == 1);

    click("combatLogClearButton");
    CHECK(model("combatLog")->rowCount() == 0);
    // The roster is untouched by either clear.
    CHECK(rosterNames() == kHerosParty);
}
