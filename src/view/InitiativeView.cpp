#include "InitiativeView.h"
#include <QStyleOptionViewItem>
#include <QAbstractItemView>
#include <QAbstractItemDelegate>
#include <QLineEdit>
#include <QDebug>
#include "InitiativeDelegate.h"

void InitiativeView::mousePressEvent(QMouseEvent* event) {
    //qDebug() << "InitiativeView::mousePressEvent(QMouseEvent* event)";
    QModelIndex index = indexAt(event->pos());
    if (index.isValid() && itemDelegate()) {
        QStyleOptionViewItem opt = makeViewOptions();
        // important for checkActiveIndex
        opt.widget = this;                        
        opt.rect = visualRect(index);
        opt.state |= QStyle::State_Active;
        opt.state |= QStyle::State_Enabled;

        //qDebug() << "Forwarding mouse release event to editor";
        itemDelegate()->editorEvent(event, model(), opt, index);
    }
    // no base call: blocks selection changes
}


void InitiativeView::mouseReleaseEvent(QMouseEvent* event) {
    //qDebug() << "InitiativeView::mouseReleaseEvent(QMouseEvent* event)";
    QModelIndex index = indexAt(event->pos());
    if (index.isValid() && itemDelegate()) {
        QStyleOptionViewItem opt = makeViewOptions();
        // important for checkActiveIndex
        opt.widget = this;                        
        opt.rect = visualRect(index);
        opt.state |= QStyle::State_Active;
        opt.state |= QStyle::State_Enabled;

        //qDebug() << "Forwarding mouse release event to editor";
        itemDelegate()->editorEvent(event, model(), opt, index);
    }
    // no base call: blocks selection changes
}

bool InitiativeView::hasActiveIndex() const { 
    //qDebug() << "InitiativeView::hasActiveIndex()=" << activeIndex_.isValid();
    return activeIndex_.isValid(); 
}

void InitiativeView::setActiveIndex(const QModelIndex& index) {
    //qDebug() << "setActiveIndex called with row" << index.row() << "valid?" << index.isValid();
    activeIndex_ = index;
    viewport()->update();
}

QModelIndex InitiativeView::activeIndex() const { return activeIndex_; }

QStyleOptionViewItem InitiativeView::makeViewOptions() const {
    QStyleOptionViewItem option;
    option.initFrom(this);
    option.font   = font();
    option.locale = locale();
    option.palette = palette();
    option.widget = this;
    return option;
}

void InitiativeView::refreshIndex(const QModelIndex& index) {
    update(visualRect(index));
}

bool InitiativeView::editCustom(const QModelIndex& index) {
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

    // Step 3: give it the current data
    itemDelegate()->setEditorData(editor, index);

    // Step 4: place it
    itemDelegate()->updateEditorGeometry(editor, option, index); // zoom into the exact viewport
    editor->show();
    editor->setFocus();


    // Step 6: update our custom active index
    activeIndex_ = index;
    viewport()->update();

    qDebug() << "editCustom: editor started at row" << index.row();
    return true;
}

void InitiativeView::setItemDelegate(QAbstractItemDelegate* delegate) {
    if (itemDelegate() == delegate)
        return;

    // Disconnect old delegate’s auto connections
    if (itemDelegate()) {
        disconnect(itemDelegate(), nullptr, this, nullptr);
    }

    QAbstractItemView::setItemDelegate(delegate);

    // Override the degault connections to QAbstractItemView’s default slots
    // Instead connect InitiativeView's handlers
    if (delegate) {
        qDebug() << "Connecting commitData and closeEditor signals.";
        connect(delegate, &QAbstractItemDelegate::commitData,
                this, &InitiativeView::onCommitData);
        connect(delegate, &QAbstractItemDelegate::closeEditor,
                this, &InitiativeView::onCloseEditor);
    }
}

void InitiativeView::onCommitData(QWidget* editor) {
    qDebug() << "InitiativeView::onCommitData()";
    auto* d = qobject_cast<InitiativeDelegate*>(itemDelegate());
    if (!d) return;

    QModelIndex idx = d->indexForEditor(editor);
    if (!idx.isValid()) {
        qDebug() << "CommitData: editor has no index mapping";
        return;
    }

    itemDelegate()->setModelData(editor, model(), idx);
}

void InitiativeView::onCloseEditor(QWidget* editor) {
    qDebug() << "InitiativeView::onCloseEditor()";
    editor->deleteLater();
    viewport()->update();
}
