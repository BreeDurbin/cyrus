#include "RosterView.h"
#include <QStyleOptionViewItem>
#include <QAbstractItemView>
#include <QAbstractItemDelegate>
#include <QLineEdit>
#include <QDebug>
#include "RosterDelegate.h"

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

    // Only one inline add editor at a time
    if (sentinelEditor_) {
        sentinelEditor_->setFocus();
        return true;
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
    placeEditor(editor, index);
    sentinelEditor_ = editor;

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

QModelIndex RosterView::indexForEditor(QWidget* editor) const {
    // The only custom editor is the inline add editor, which belongs to the
    // sentinel row (always the last row).
    if (!editor || editor != sentinelEditor_ || !model() || model()->rowCount() == 0)
        return QModelIndex();
    return model()->index(model()->rowCount() - 1, 0);
}

void RosterView::onCommitData(QWidget* editor) {
    auto* d = qobject_cast<RosterDelegate*>(itemDelegate());
    if (!d) return;

    QModelIndex idx = indexForEditor(editor);
    if (!idx.isValid()) {
        qDebug() << "CommitData: editor has no index mapping";
        return;
    }

    itemDelegate()->setModelData(editor, model(), idx);
}

void RosterView::onCloseEditor(QWidget* editor) {
    if (editor == sentinelEditor_) sentinelEditor_.clear();
    editor->deleteLater();
    viewport()->update();
}

void RosterView::updateEditorGeometries() {
    QListView::updateEditorGeometries();

    // Rows were added/removed or the view resized: move the inline add editor to
    // the sentinel row, which is always the last row.
    if (sentinelEditor_ && model() && model()->rowCount() > 0) {
        placeEditor(sentinelEditor_, model()->index(model()->rowCount() - 1, 0));
    }
}

void RosterView::placeEditor(QWidget* editor, const QModelIndex& index) {
    QStyleOptionViewItem option = makeViewOptions();
    option.rect = visualRect(index);
    option.state |= QStyle::State_Enabled | QStyle::State_Active;
    itemDelegate()->updateEditorGeometry(editor, option, index);
}
