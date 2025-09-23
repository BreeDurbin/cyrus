#pragma once
#include <QListView>
#include <QMouseEvent>
#include <QPersistentModelIndex>

class RosterView : public QListView {
    Q_OBJECT
    public:
        explicit RosterView(QWidget* parent = nullptr)
            : QListView(parent) {}

        QStyleOptionViewItem makeViewOptions() const;
        void refreshIndex(const QModelIndex& index);
        bool editCustom(const QModelIndex& index);
        void setItemDelegate(QAbstractItemDelegate* delegate);
        QModelIndex indexForEditor(QWidget* editor) const;

    public slots:
        void onCommitData(QWidget* editor);
        void onCloseEditor(QWidget* editor);

};
