#pragma once

#include <QStyledItemDelegate>
#include <QPoint>
#include <QMap>
#include <QUuid>
#include "Character.h"
#include "CharacterController.h"

class RosterDelegate : public QStyledItemDelegate {
    Q_OBJECT

    public:
        explicit RosterDelegate(CharacterController* characterController, QObject *parent = nullptr);

        void paint(QPainter* painter,
                const QStyleOptionViewItem& option,
                const QModelIndex& index) const override;

        bool editorEvent(QEvent *event,
                        QAbstractItemModel *model,
                        const QStyleOptionViewItem &option,
                        const QModelIndex &index) override;
        QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    
    signals:
        void incrementRosterMemberInitiativeClicked(const QModelIndex& index);
        void decrementRosterMemberInitiativeClicked(const QModelIndex& index);
        void cloneRosterMemberClicked(const QModelIndex& index);
        void deleteButtonClicked(const QModelIndex& index);

    protected:
        QPoint cursorPosInItem(const QStyleOptionViewItem& option, const QEvent* event = nullptr) const;
        int minimumWidth(const Character& character) const;

    private:
        CharacterController* characterController_;

};
