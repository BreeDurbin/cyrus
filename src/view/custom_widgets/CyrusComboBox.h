#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QToolButton>
#include <QMenu>
#include <QAction>
#include "CyrusToolButton.h"

class CyrusComboBox : public QWidget
{
    Q_OBJECT

public:
    explicit CyrusComboBox(QWidget* parent = nullptr);

    void addItem(const QString& text, const QVariant& userData = QVariant());
    void addItems(const QStringList& items);

    QString currentText() const;
    QVariant currentData() const;

    void setText(const QString& text);
    void setCurrentIndex(int index);
    QLineEdit* lineEdit() const { return edit_; }

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void currentIndexChanged(const QString& text);
    void currentIndexChanged(int index);
    void textChanged(const QString& text);

private slots:
    void onActionTriggered(QAction* action);
    void onTextEdited(const QString& text);

private:
    QLineEdit* edit_;
    CyrusToolButton* button_;
    QMenu* menu_;
    QList<QAction*> actions_;
    int currentIndex_ = -1;

    QString style() const;
};
