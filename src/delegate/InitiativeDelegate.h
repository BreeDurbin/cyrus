#pragma once

#include <QStyledItemDelegate>
#include <QPoint>
#include <QMap>
#include <QUuid>
#include "Character.h"
#include "CharacterController.h"
#include <QLineEdit>
#include <QAbstractItemView>
#include <QPointer>
#include <QWidget>
#include <QPersistentModelIndex>
#include <QHash>


class InitiativeDelegate : public QStyledItemDelegate {
    Q_OBJECT

    public:
        explicit InitiativeDelegate(CharacterController* characterController, 
                                    QAbstractItemView* view, 
                                    QObject* parent = nullptr)
                                      : QStyledItemDelegate{parent},
                                        characterController_{characterController},
                                        view_{view} {};

        void paint(QPainter* painter,
                const QStyleOptionViewItem& option,
                const QModelIndex& index) const override;

        bool editorEvent(QEvent *event,
                        QAbstractItemModel *model,
                        const QStyleOptionViewItem &option,
                        const QModelIndex &index) override;

        QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
        void setEditorData(QWidget* editor, const QModelIndex& index) const override;
        void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
        void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
        QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
        QAbstractItemView* view() const { return view_; }
        QWidget* editorForIndex(const QModelIndex& index) const;
        QModelIndex indexForEditor(QWidget* editor) const;

    signals:

        //Cast Actions
        void spellNameEdited(const QUuid& id, const QString& name) const;
        void decrementCastingTime(const QUuid& id);
        void incrementCastingTime(const QUuid& id);
        void decrementDuration(const QUuid& id);
        void incrementDuration(const QUuid& id);
        void castSubmitted(const QUuid& id, const std::shared_ptr<Character>& character);

        //Attack Actions
        void decrementAttackAmount(const QModelIndex& index);
        void incrementAttackAmount(const QModelIndex& index);
        void decrementWeaponSpeed(const QModelIndex& index);
        void incrementWeaponSpeed(const QModelIndex& index);
        void attackSubmitted(const QModelIndex& index);

        // Misc Actions
        void miscSubmitted(const QModelIndex& index);
        void iconSelected(const QModelIndex& index, Cyrus::MiscActionType miscActionType); // icon selector hit

        //character model
        void deleteItem(const QModelIndex& index) const; // delete button clicked
        void iconSelected(const QModelIndex& index, Cyrus::ActionType actionType); // icon selector hit
        void cancelAction(const QModelIndex& index);
        void decrementInitiative(const QModelIndex& index);
        void incrementInitiative(const QModelIndex& index);

    protected:
        bool checkSelectedIndex(const QModelIndex& index, const QStyleOptionViewItem& option, 
                const Cyrus::CombatSequenceState state, const std::shared_ptr<Character>& character) const;
        bool checkExpanded(bool isActiveIndex, std::shared_ptr<Character> character) const;
        bool checkCurrentInitiative(const QModelIndex& index, int currentInitiative) const;
        bool checkHovered(const QStyleOptionViewItem &option) const;
        bool checkIfCharacter(std::shared_ptr<Character> character) const;
        QPoint cursorPosInItem(const QStyleOptionViewItem& option, const QEvent* event = nullptr) const;
        QColor backgroundForState(const Cyrus::CombatSequenceState state, const std::shared_ptr<Character>& character, 
                                                    bool isActiveIndex, bool isCurrentInitiative) const;

    private:
        mutable QHash<QPersistentModelIndex, QPointer<QWidget>> editors_;
        QAbstractItemView* view_;
        CharacterController* characterController_;

};
