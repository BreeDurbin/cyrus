#pragma once

#include <QStyledItemDelegate>
#include <QPoint>
#include <QMap>
#include <QUuid>
#include "Character.h"
#include "CharacterController.h"
#include <QAbstractItemView>

class RosterDelegate : public QStyledItemDelegate {
    Q_OBJECT

    public:
        explicit RosterDelegate(CharacterController* characterController, QAbstractItemView* view, QObject *parent = nullptr);

        void paint(QPainter* painter,
                const QStyleOptionViewItem& option,
                const QModelIndex& index) const override;

        bool editorEvent(QEvent *event,
                        QAbstractItemModel *model,
                        const QStyleOptionViewItem &option,
                        const QModelIndex &index) override;

        QWidget* createEditor(QWidget* parent,
                                      const QStyleOptionViewItem& option,
                                      const QModelIndex& index) const override;

        void updateEditorGeometry(QWidget* editor,
                                          const QStyleOptionViewItem& option,
                                          const QModelIndex& index) const override;

        QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
        QAbstractItemView* view() const { return view_; }
    
    signals:
        void nextFaction(const QModelIndex& index);
        void previousFaction(const QModelIndex& index);
        void cloneRosterMember(const QModelIndex& index);
        void deleteRosterMember(const QModelIndex& index);
        void characterCreated(const QString &name);

    protected:
        QPoint cursorPosInItem(const QStyleOptionViewItem& option, const QEvent* event = nullptr) const;
        int minimumWidth(const Character& character) const;

    private:
        CharacterController* characterController_;
        QAbstractItemView* view_;

};
