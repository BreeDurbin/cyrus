#pragma once
#include <QListView>
#include <QMouseEvent>
#include <QPersistentModelIndex>
#include <QPointer>

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

    protected slots:
        // Also keeps the inline add editor on the sentinel row, which is always last.
        void updateEditorGeometries() override;

    private:
        // editCustom() creates this editor itself, so the base view doesn't track it.
        QPointer<QWidget> sentinelEditor_;
        void placeEditor(QWidget* editor, const QModelIndex& index);

};
