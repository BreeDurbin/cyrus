#include "InitiativeView.h"
#include <QStyleOptionViewItem>
#include <QAbstractItemView>

void InitiativeView::mousePressEvent(QMouseEvent* event) {
    qDebug() << "InitiativeView::mousePressEvent(QMouseEvent* event)";
    QModelIndex index = indexAt(event->pos());
    if (index.isValid() && itemDelegate()) {
        QStyleOptionViewItem opt = makeViewOptions();
        // important for checkActiveIndex
        opt.widget = this;                        
        opt.rect = visualRect(index);
        opt.state |= QStyle::State_Active;
        opt.state |= QStyle::State_Enabled;

        qDebug() << "Forwarding mouse release event to editor";
        itemDelegate()->editorEvent(event, model(), opt, index);
    }
    // no base call: blocks selection changes
}


void InitiativeView::mouseReleaseEvent(QMouseEvent* event) {
    qDebug() << "InitiativeView::mouseReleaseEvent(QMouseEvent* event)";
    QModelIndex index = indexAt(event->pos());
    if (index.isValid() && itemDelegate()) {
        QStyleOptionViewItem opt = makeViewOptions();
        // important for checkActiveIndex
        opt.widget = this;                        
        opt.rect = visualRect(index);
        opt.state |= QStyle::State_Active;
        opt.state |= QStyle::State_Enabled;

        qDebug() << "Forwarding mouse release event to editor";
        itemDelegate()->editorEvent(event, model(), opt, index);
    }
    // no base call: blocks selection changes
}

bool InitiativeView::hasActiveIndex() const { return activeIndex_.isValid(); }

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
