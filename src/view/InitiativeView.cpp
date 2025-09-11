#include "InitiativeView.h"
#include <QStyleOptionViewItem>
#include <QAbstractItemView>
#include <QAbstractItemDelegate>
#include <QLineEdit>
#include <QDebug>

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
    qDebug() << "InitiativeView::hasActiveIndex()=" << activeIndex_.isValid();
    return activeIndex_.isValid(); 
}

void InitiativeView::setActiveIndex(const QModelIndex& index) {
    qDebug() << "setActiveIndex called with row" << index.row() << "valid?" << index.isValid();
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

    // Step 5: connect commit/close cycle
    connect(editor, &QWidget::destroyed, this, [=]() {
        qDebug() << "editCustom: editor destroyed for row" << index.row();
    });

    // If it’s a QLineEdit, hook commit
    if (auto* line = qobject_cast<QLineEdit*>(editor)) {
        connect(line, &QLineEdit::editingFinished, this, [this, line, index, editor]() {
            qDebug() << "editCustom: committing data for row" << index.row();
            itemDelegate()->setModelData(line, model(), index);
            editor->deleteLater();
        });
    }

    // Step 6: update our custom active index
    activeIndex_ = index;
    viewport()->update();

    qDebug() << "editCustom: editor started at row" << index.row();
    return true;
}

