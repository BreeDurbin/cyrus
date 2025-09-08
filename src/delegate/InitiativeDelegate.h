#pragma once

#include <QStyledItemDelegate>
#include <QPoint>
#include <QMap>
#include <QUuid>
#include "Character.h"

class InitiativeDelegate : public QStyledItemDelegate {
    Q_OBJECT

    public:
        explicit InitiativeDelegate(QObject *parent = nullptr);

        void paint(QPainter* painter,
                const QStyleOptionViewItem& option,
                const QModelIndex& index) const override;

        bool editorEvent(QEvent *event,
                        QAbstractItemModel *model,
                        const QStyleOptionViewItem &option,
                        const QModelIndex &index) override;

        QSize sizeHint(const QStyleOptionViewItem& option,
                    const QModelIndex& index) const override;

    signals:
        void deleteItem(const QModelIndex& index) const;
        void requestSpellNameEdit(const QModelIndex& index,
                                const QString& currentName) const;
        void castSubmitted(const QModelIndex& index,
                        const CastState& state) const;

    protected:

        // Helpers
        bool checkActiveIndex(const QStyleOptionViewItem& option,
                            const QModelIndex& index) const;
        bool checkExpanded(bool isActiveIndex,
                        Cyrus::ActionType actionType) const;
        bool checkCurrentSegment(const QModelIndex& index,
                                int currentInitiative) const;
        bool checkHovered(const QStyleOptionViewItem &option) const;

        QPoint cursorPosInItem(const QStyleOptionViewItem& option,
                            const QEvent* event = nullptr) const;

    private:
        bool isExpanded(const QModelIndex& index) const;
        bool isActiveIndex(const QModelIndex& index) const;
};
