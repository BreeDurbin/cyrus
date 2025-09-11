#include "StyleRepository.h"
#include "ColorRepository.h"

#include <QString>
#include <QStringLiteral>
#include <QColor>
#include <QIcon>

namespace {
    // Helper to convert QColor to CSS hex string (#RRGGBB)
    QString cssColor(const QColor &c) {
        return c.name(QColor::HexRgb);
    }
}


QString StyleRepository::hero() {
    return QStringLiteral(R"(
        QFrame#heroFrame { 
            background-image: url(:/image/CYRUS_HERO.png);
            background-position: left;
            background-repeat: no-repeat;
            background-size: cover;   /* scale to cover entire frame */
        }
    )");
}

QString StyleRepository::mainWindow() {
    return QStringLiteral(R"(
        QDialog, QMainWindow {
            background: %1;
            border: 2px solid %2;
            border-radius: 8px;
            padding: 8px;
        }

        QDialog::title, QMainWindow::title {
            color: %3;
            font-family: "Segoe UI", "Helvetica Neue", sans-serif;
            font-size: 16pt;
            font-weight: bold;
        }
    )")
        .arg(cssColor(ColorRepository::windowBackground()))
        .arg(cssColor(ColorRepository::baseBackground()))
        .arg(cssColor(ColorRepository::text()));
}

QString StyleRepository::label() {
    return QStringLiteral(R"(
        QLabel {
            background: transparent;
            color: %1;
            font-family: "Segoe UI", "Helvetica Neue", sans-serif;
            font-size: 24px;
            padding: 2px;
        }
        QLabel:disabled {
            color: %2;
        }
    )")
        .arg(cssColor(ColorRepository::text()))
        .arg(cssColor(ColorRepository::pressedOutlineColor())); // using muted/disabled tone
}

QString StyleRepository::itemView() {
    return QStringLiteral(R"(
        QListView, QTreeView, QTableView {
            background: %1;
            border: 1px solid %2;
            border-radius: 6px;
            padding: 4px;
            outline: 0;
            selection-background-color: transparent;
        }

        QListView::item, QTreeView::item, QTableView::item {
            background: transparent;
            border-radius: 4px;
            padding: 6px 10px;
            margin: 2px 4px;
            color: %3;
            font-family: "Segoe UI", "Helvetica Neue", sans-serif;
            font-size: 13px;
        }

        QListView::item:hover, QTreeView::item:hover, QTableView::item:hover {
            background: %4;
        }

        QListView::item:selected, QTreeView::item:selected, QTableView::item:selected {
            background: %5;
            color: %6;
            border: 1px solid %7;
        }

        QHeaderView::section {
            background: %1;
            border: 1px solid %2;
            padding: 6px 10px;
            font-weight: bold;
            font-family: "Segoe UI", "Helvetica Neue", sans-serif;
            color: %8;
        }
    )")
        .arg(cssColor(ColorRepository::baseBackground()))       // %1
        .arg(cssColor(ColorRepository::pressedOutlineColor()))  // %2
        .arg(cssColor(ColorRepository::text()))                 // %3
        .arg(cssColor(ColorRepository::buttonBackground()))     // %4 hover
        .arg(cssColor(ColorRepository::selectedBackground()))   // %5 selected background
        .arg(cssColor(ColorRepository::pressedTextColor()))     // %6 selected text
        .arg(cssColor(ColorRepository::buttonOutlineColor()))   // %7 selected border
        .arg(cssColor(ColorRepository::selectedForeground()));  // %8 header text
}

QString StyleRepository::pushButton() {
    return QStringLiteral(R"(
        QPushButton {
            background-color: %1;
            color: %2;
            border: 1px solid %3;
            border-radius: 6px;
            padding: 6px 12px;
            font-size: 13px;
            font-weight: bold;
        }

        QPushButton:hover {
            background-color: %4;
            border: 1px solid %5;
        }

        QPushButton:pressed {
            background-color: %6;
            border: 1px solid %7;
            color: %8;
        }

        QPushButton:disabled {
            background-color: %9;
            color: %10;
            border: 1px solid %11;
        }
    )")
        .arg(cssColor(ColorRepository::selectedBackground()))  // %1 background
        .arg(cssColor(ColorRepository::text()))                // %2 text
        .arg(cssColor(ColorRepository::buttonOutlineColor()))  // %3 border
        .arg(cssColor(ColorRepository::hoverTextColor()))      // %4 hover bg
        .arg(cssColor(ColorRepository::selectedForeground()))  // %5 hover border
        .arg(cssColor(ColorRepository::buttonPressedBackground())) // %6 pressed bg
        .arg(cssColor(ColorRepository::pressedOutlineColor())) // %7 pressed border
        .arg(cssColor(ColorRepository::windowBackground()))    // %8 pressed text
        .arg(cssColor(ColorRepository::buttonBackground()))    // %9 disabled bg
        .arg(cssColor(ColorRepository::hoverTextColor()))      // %10 disabled text
        .arg(cssColor(ColorRepository::baseBackground()));     // %11 disabled border
}

QString StyleRepository::iconPushButton() {
    QString style = QString(R"(
        QPushButton {
            background: transparent;
            border: none;
            padding: 0px;
        }
        QPushButton:hover {
            background: transparent;
            border: 2px solid %1;   /* gold outline from repo */
            border-radius: 8px;     /* more rounded corners */
        }
        QPushButton:pressed {   
            background-color: rgba(0, 0, 0, 60); /* darker press effect */
            border: 2px solid %2;   /* pressed outline from repo */
            border-radius: 8px;
        }
    )")
    .arg(ColorRepository::buttonOutlineColor().name())    // hover outline
    .arg(ColorRepository::pressedOutlineColor().name());  // pressed outline

    return style;
}

QString StyleRepository::spinBox() {
    return QStringLiteral(R"(
        QSpinBox {
            background-color: %1;
            color: %2;
            border: 1px solid %3;
            border-radius: 6px;
            padding: 4px 8px;
            selection-background-color: %4;
            selection-color: %5;
        }
        QSpinBox::up-button {
            subcontrol-origin: border;
            subcontrol-position: top right;
            width: 16px;
            background: %6;
            border-left: 1px solid %3;
            border-top-right-radius: 6px;
        }
        QSpinBox::up-button:hover {
            background: %7;
        }
        QSpinBox::down-button {
            subcontrol-origin: border;
            subcontrol-position: bottom right;
            width: 16px;
            background: %6;
            border-left: 1px solid %3;
            border-bottom-right-radius: 6px;
        }
        QSpinBox::down-button:hover {
            background: %7;
        }
    )")
    .arg(ColorRepository::baseBackground().name())        // %1 background
    .arg(ColorRepository::text().name())                  // %2 text
    .arg(ColorRepository::buttonOutlineColor().name())    // %3 border
    .arg(ColorRepository::selectedBackground().name())    // %4 selection bg
    .arg(ColorRepository::selectedForeground().name())    // %5 selection fg
    .arg(ColorRepository::buttonBackground().name())      // %6 arrow bg
    .arg(ColorRepository::buttonHoveredBackground().name()); // %7 hover
}

QString StyleRepository::lineEdit() {
    return QStringLiteral(R"(
        QLineEdit {
            background-color: %1;
            color: %2;
            border: 1px solid %3;
            border-radius: 6px;
            padding: 4px 8px;
            selection-background-color: %4;
            selection-color: %5;
        }
        QLineEdit:focus {
            border: 1px solid %6;
        }
        QLineEdit:disabled {
            color: %7;
            background: %8;
        }
        QLineEdit:placeholder {
            color: %9;
            font-style: italic;
        }
    )")
    .arg(ColorRepository::baseBackground().name())         // %1 background
    .arg(ColorRepository::text().name())                   // %2 text
    .arg(ColorRepository::buttonOutlineColor().name())     // %3 border
    .arg(ColorRepository::selectedBackground().name())     // %4 selection bg
    .arg(ColorRepository::selectedForeground().name())     // %5 selection fg
    .arg(ColorRepository::pressedOutlineColor().name())    // %6 focus border
    .arg(ColorRepository::hoverTextColor().name())         // %7 disabled text
    .arg(ColorRepository::buttonBackground().name())       // %8 disabled bg
    .arg(ColorRepository::placeholderText().name());       // %9 placeholder text
}

QString StyleRepository::comboBox() {
    return QStringLiteral(R"(
        QComboBox {
            background-color: %1;
            color: %2;
            border: 1px solid %3;
            border-radius: 6px;
            padding: 4px 8px;
            font-family: "Segoe UI", "Helvetica Neue", sans-serif;
            font-size: 13px;
        }

        QComboBox:hover {
            background-color: %4;
            border: 1px solid %5;
        }

        QComboBox:focus {
            border: 1px solid %6;
        }

        QComboBox:disabled {
            background-color: %7;
            color: %8;
            border: 1px solid %3;
        }

        QComboBox::drop-down {
            subcontrol-origin: padding;
            subcontrol-position: top right;
            width: 20px;
            border-left: 1px solid %3;
            border-radius: 0 6px 6px 0;
            background-color: %1;
        }

        QComboBox::down-arrow {
            image: url(:/icons/arrow_down.png);
            width: 12px;
            height: 12px;
        }

        QComboBox QAbstractItemView {
            background-color: %1;
            border: 1px solid %3;
            selection-background-color: %9;
            selection-color: %10;
            outline: 0;
            padding: 2px;
        }

        QComboBox QAbstractItemView::item {
            padding: 4px 8px;
            color: %2;
        }

        QComboBox QAbstractItemView::item:hover {
            background-color: %4;
        }

        QComboBox QAbstractItemView::item:selected {
            background-color: %9;
            color: %10;
        }
    )")
    .arg(ColorRepository::baseBackground().name())        // %1 background
    .arg(ColorRepository::text().name())                  // %2 text
    .arg(ColorRepository::buttonOutlineColor().name())    // %3 border
    .arg(ColorRepository::buttonHoveredBackground().name()) // %4 hover bg
    .arg(ColorRepository::hoverTextColor().name())        // %5 hover border
    .arg(ColorRepository::pressedOutlineColor().name())   // %6 focus border
    .arg(ColorRepository::buttonBackground().name())      // %7 disabled bg
    .arg(ColorRepository::hoverTextColor().name())        // %8 disabled text
    .arg(ColorRepository::selectedBackground().name())    // %9 selected background
    .arg(ColorRepository::selectedForeground().name());   // %10 selected text
}


QFont StyleRepository::labelFont(int pointSize, bool bold)
{
    QFont font("Segoe UI", pointSize);   // modern, clean font
    font.setBold(bold);
    return font;
}

QFont StyleRepository::labelPlaceholderFont(int pointSize)
{
    QFont font("Segoe UI", pointSize);   // modern, clean font
    font.setItalic(true);
    return font;
}

QFont StyleRepository::progressFont(int pointSize, bool bold)
{
    QFont font("Segoe UI", pointSize);   // modern, clean font
    font.setBold(bold);
    return font;
}

int StyleRepository::progressArcWidth() {
    return 30;
}
