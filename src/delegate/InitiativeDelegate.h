#pragma once

#include <QStyledItemDelegate>
#include <QPoint>
#include <QMap>
#include <QUuid>
#include "Character.h"
#include "CharacterController.h"
#include <QLineEdit>

class InitiativeDelegate : public QStyledItemDelegate {
    Q_OBJECT

    public:
        explicit InitiativeDelegate(CharacterController* characterController, QObject *parent = nullptr);

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

    signals:
        //update item state 
        void commitData(const QLineEdit* lineEdit) const;
        void closeEditor(const QLineEdit* lineEdit) const;
        void spellNameEdited(const QUuid& id, const QString& name) const;
        void decrementCastingTimeClicked(const QUuid& id);
        void incrementCastingTimeClicked(const QUuid& id);
        void decrementDurationClicked(const QUuid& id);
        void incrementDurationClicked(const QUuid& id);
        void castSubmitted(const QUuid& id, const Character& character);

        //update model
        void deleteItemClicked(const QModelIndex& index) const; // delete button clicked
        void iconSelectorClicked(const QModelIndex& index, Cyrus::ActionType actionType); // icon selector hit

    protected:
        bool checkActiveIndex(const QModelIndex& index, const QStyleOptionViewItem& option) const;
        bool checkExpanded(bool isActiveIndex, std::shared_ptr<Character> character) const;
        bool checkCurrentSegment(const QModelIndex& index, int currentInitiative) const;
        bool checkHovered(const QStyleOptionViewItem &option) const;
        bool checkIfCharacter(std::shared_ptr<Character> character) const;
        QPoint cursorPosInItem(const QStyleOptionViewItem& option, const QEvent* event = nullptr) const;

    private:
        CharacterController* characterController_;

};
