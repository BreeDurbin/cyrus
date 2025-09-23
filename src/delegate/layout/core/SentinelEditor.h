#pragma once

#include <QWidget>
#include<QPushButton>
#include<QLineEdit>

class SentinelEditor : public QWidget {
    Q_OBJECT
public:
    explicit SentinelEditor(QWidget* parent = nullptr);

    QLineEdit* lineEdit() const { return lineEdit_; }
    QPushButton* addButton() const { return addButton_; }

    QSize sizeHint() const override {
        return QSize(200, 32);
    }

private:
    QLineEdit* lineEdit_;
    QPushButton* addButton_;
};