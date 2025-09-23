#pragma once
#include <QListView>
#include <QMouseEvent>
#include <QPersistentModelIndex>

class InitiativeView : public QListView {
    Q_OBJECT
public:
    explicit InitiativeView(QWidget* parent = nullptr)
        : QListView(parent) {}

    bool hasActiveIndex() const;
    void setActiveIndex(const QModelIndex& idx);
    QModelIndex activeIndex() const;
    QStyleOptionViewItem makeViewOptions() const;
    void refreshIndex(const QModelIndex& index);
    bool editCustom(const QModelIndex& index);
    void setItemDelegate(QAbstractItemDelegate* delegate);
    QModelIndex indexForEditor(QWidget* editor) const;



public slots:
    void onCommitData(QWidget* editor);
    void onCloseEditor(QWidget* editor);

protected:
    QPersistentModelIndex activeIndex_; 
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
};
