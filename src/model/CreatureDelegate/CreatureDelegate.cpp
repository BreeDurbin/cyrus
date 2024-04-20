#include "CreatureDelegate.h"
#include "../Creature/Creature.h"
#include "../CreatureEditor/CreatureEditor.h"

QWidget *CreatureDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem & /*option*/,
                                     const QModelIndex & /*index*/) const
{
    return new CreatureEditor(parent);
}

void CreatureDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    static_cast<CreatureEditor *>(editor)->setValue(index.data().value<Creature>());
}

void CreatureDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
    model->setData(index, QVariant::fromValue(static_cast<CreatureEditor *>(editor)->value()));
}

void CreatureDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                         const QModelIndex & /*index*/) const
{
    const int hCell = option.rect.height();
    const int hEditor = editor->sizeHint().height();
    const int h = qMax(hCell, hEditor);
    QSize size = option.rect.size();
    size.setHeight(h);
    editor->setGeometry(QRect(option.rect.topLeft() - QPoint(0, (h - hCell) / 2), size));
}