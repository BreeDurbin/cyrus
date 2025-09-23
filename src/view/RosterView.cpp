#include "RosterView.h"
#include <QStyleOptionViewItem>
#include <QAbstractItemView>
#include <QAbstractItemDelegate>
#include <QLineEdit>
#include <QDebug>
#include "InitiativeDelegate.h"

QStyleOptionViewItem RosterView::makeViewOptions() const {
    QStyleOptionViewItem option;
    option.initFrom(this);
    option.font   = font();
    option.locale = locale();
    option.palette = palette();
    option.widget = this;
    return option;
}

void RosterView::refreshIndex(const QModelIndex& index) {
    update(visualRect(index));
}

bool RosterView::editCustom(const QModelIndex& index) {
    if (!index.isValid()) {
        qDebug() << "editCustom: invalid index";
        return false;
    }
    if (!model() || !itemDelegate()) {
        qDebug() << "editCustom: no model or delegate";
        return false;
    }

    // Step 1: prepare style option
    QStyleOptionViewItem option = this->makeViewOptions();
    option.rect = visualRect(index);
    option.state |= QStyle::State_Enabled | QStyle::State_Active;

    // Step 2: let the delegate create the editor
    QWidget* editor = itemDelegate()->createEditor(viewport(), option, index);
    if (!editor) {
        qDebug() << "editCustom: delegate refused editor";
        return false;
    }

    qDebug() << "EDIT CUSTOM ROSTER VIEW";

    // Step 3: give it the current data
    itemDelegate()->setEditorData(editor, index);

    // Step 4: place it
    itemDelegate()->updateEditorGeometry(editor, option, index);

    // this does not show if the row index is > 0
    editor->show();
    editor->setFocus();

    viewport()->update();

    qDebug() << "editCustom: editor started at row" << index.row();
    return true;
}

void RosterView::setItemDelegate(QAbstractItemDelegate* delegate) {
    if (itemDelegate() == delegate)
        return;

    QAbstractItemView::setItemDelegate(delegate);

    // Disconnect old delegate’s auto connections
    if (itemDelegate()) {
        disconnect(itemDelegate(), nullptr, this, nullptr);
    }

    // Override the degault connections to QAbstractItemView’s default slots
    // Instead connect InitiativeView's handlers
    if (delegate) {
        qDebug() << "Connecting commitData and closeEditor signals.";
        connect(delegate, &QAbstractItemDelegate::commitData,
                this, &RosterView::onCommitData);
        connect(delegate, &QAbstractItemDelegate::closeEditor,
                this, &RosterView::onCloseEditor);
    }
}

void RosterView::onCommitData(QWidget* editor) {
    auto* d = qobject_cast<InitiativeDelegate*>(itemDelegate());
    if (!d) return;

    QModelIndex idx = d->indexForEditor(editor);
    if (!idx.isValid()) {
        qDebug() << "CommitData: editor has no index mapping";
        return;
    }

    itemDelegate()->setModelData(editor, model(), idx);
}

void RosterView::onCloseEditor(QWidget* editor) {
    editor->deleteLater();
    viewport()->update();
}
