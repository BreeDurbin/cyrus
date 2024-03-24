#ifndef DRACULA_H
#define DRACULA_H

#include <QString>

static const QString draculaStyleComboBox = R"(
                QComboBox {
                    background-color: #282a36;
                    color: #f8f8f2;
                    border: 1px solid #6272a4;
                    padding: 5px;
                }
                QComboBox::drop-down {
                    subcontrol-origin: padding;
                    subcontrol-position: top right;
                    width: 20px;
                    border-left-width: 1px;
                    border-left-color: #6272a4;
                    border-left-style: solid;
                    border-top-right-radius: 3px;
                    border-bottom-right-radius: 3px;
                    background-color: #44475a;
                }
                QComboBox::down-arrow {
                    image: url(:/icons/down_arrow.png);
                }
                QComboBox::down-arrow:on {
                    image: url(:/icons/up_arrow.png);
                }
                QComboBox QAbstractItemView {
                    background-color: #282a36;
                    color: #f8f8f2;
                    selection-background-color: #44475a;
                    selection-color: #f8f8f2;
                }
                QComboBox::item {
                    padding: 5px;
                }
                QComboBox::item:selected {
                    background-color: #44475a;
                }
                QComboBox::item:hover {
                    background-color: #6272a4;
                }
            )";

static const QString draculaStylePushButton = R"(
                QPushButton {
                    background-color: #282a36;
                    color: #f8f8f2;
                    border: 1px solid #6272a4;
                    padding: 5px;
                }
                QPushButton:hover {
                    background-color: #6272a4;
                }
                QPushButton:pressed {
                    background-color: #44475a;
                }
            )";

static const QString draculaStyleListView = R"(
                QListView {
                    background-color: #282a36;
                    color: #f8f8f2;
                    border: 1px solid #6272a4;
                    padding: 5px;
                }
                QListView::item {
                    padding: 5px;
                }
                QListView::item:selected {
                    background-color: #44475a;
                }
                QListView::item:hover {
                    background-color: #6272a4;
                }
            )";

static const QString draculaStyleMainWindow = R"(
                QMainWindow {
                    background-color: #282a36;
                    color: #f8f8f2;
                }
            )";


#endif // DRACULA_H