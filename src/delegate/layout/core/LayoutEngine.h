#pragma once

#include "Character.h"
#include "Enums.h"
#include <QRect>
#include <QVector>
#include <QFontMetrics>
#include "CastState.h"
#include <memory>
#include <QListView>

struct Layout { 
    virtual ~Layout() = default; 
    QRect baseRect;
    QRect mainRowRect;
};

// view needed for edit support
struct HitCommand {
    std::function<void(QListView*)> execute;
};

struct LayoutEngine : public QObject {
    Q_OBJECT
    public:
        virtual ~LayoutEngine() = default;

        virtual std::shared_ptr<Layout> calculate(
            const QRect& baseRect,
            const std::shared_ptr<Character>& character,
            bool isActive,
            bool isExpanded,
            const Cyrus::CombatSequenceState state = Cyrus::CombatSequenceState::NPC_DETERMINATION
        ) const = 0;

        virtual std::optional<HitCommand> hitTest(
            const QModelIndex& index,
            const std::shared_ptr<Layout>& layout,
            const std::shared_ptr<Character>& character,
            const QPoint& cursor,
            const Cyrus::CombatSequenceState state = Cyrus::CombatSequenceState::NPC_DETERMINATION
        ) = 0;

        virtual void paintLayout(QPainter* painter,
            const std::shared_ptr<Layout>& layout,
            const std::shared_ptr<Character>& character,
            const CastState castState,
            bool isSelectedIndex,
            bool isExpanded,
            const QPoint& localCursor,
            const Cyrus::CombatSequenceState state = Cyrus::CombatSequenceState::NPC_DETERMINATION
         ) const = 0;

        virtual int minimumWidth(
            const std::shared_ptr<Character>& character) const = 0;

};
