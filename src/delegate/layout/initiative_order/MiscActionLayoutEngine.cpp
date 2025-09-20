#include "MiscActionLayoutEngine.h"

#include "PainterUtils.h"
#include "IconRepository.h"
#include "StyleRepository.h"
#include "ColorRepository.h"
#include <QApplication>
#include "LayoutUtils.h"

std::shared_ptr<Layout> MiscActionLayoutEngine::calculate(
    const QRect& rect,
    const std::shared_ptr<Character>& character,
    bool isSelectedIndex,
    bool isExpanded,
    const Cyrus::CombatSequenceState state
) const {
    MiscActionLayout layout;
    auto spec = character->layoutSpec();
    int padding = spec.padding;

    // --- base + main row ---
    layout.baseRect     = LayoutUtils::padRectangle(rect, padding); //unpadded, used to build the top and bottom row rects
    layout.mainRowRect  = LayoutUtils::buildMainRowRect(rect, false, padding);

    // --- left-hand controls (hero + initiative) ---
    LayoutUtils::LeftAnchorBuilder left(layout.mainRowRect);
    layout.heroIconRect   = left.icon(spec.heroIconScale, padding);
    layout.initiativeRect = left.text(spec.initiativeWidth, padding);

    // --- right-hand controls (delete button only for now) ---
    LayoutUtils::RightAnchorBuilder right(layout.mainRowRect);
    layout.deleteButtonRect = right.icon(spec.deleteButtonScale, padding);

    // --- text (fills remaining space between initiative and delete) ---
    int textWidth = layout.deleteButtonRect.left() - layout.initiativeRect.right() - padding;
    layout.textRect = left.text(textWidth, padding);

    Q_UNUSED(isExpanded)

    return std::make_shared<MiscActionLayout>(layout);
}


std::optional<HitCommand> MiscActionLayoutEngine::hitTest(
                        const QModelIndex& index,
                        const std::shared_ptr<Layout>& layout,
                        const std::shared_ptr<Character>& character,
                        const QPoint& cursorPos,
                        const Cyrus::CombatSequenceState state
                    ) {

    auto cLayout = std::static_pointer_cast<MiscActionLayout>(layout);

    // Delete button
    if (cLayout->deleteButtonRect.contains(cursorPos)) {
        return HitCommand{ [index, this](QListView* /*view*/) {
            qDebug() << "Delete button clicked.";
            emit deleteItemClicked(index);
        }};
    }

    return std::nullopt;
}

void MiscActionLayoutEngine::paintLayout(
                    QPainter* painter,
                    const std::shared_ptr<Layout>& layout,
                    const std::shared_ptr<Character>& character,
                    const CastState castState,
                    bool isSelectedIndex,
                    bool isExpanded,
                    const QPoint& localCursor,
                    const Cyrus::CombatSequenceState state
                ) const {
    auto cLayout = std::static_pointer_cast<MiscActionLayout>(layout);
    auto spec = character->layoutSpec();

    bool isPressed = QApplication::mouseButtons() & Qt::LeftButton;

    // --- Hero icon ---
    QIcon heroIcon = character->icon();
    heroIcon.paint(
        painter,
        cLayout->heroIconRect,
        Qt::AlignCenter,
        QIcon::Normal,
        QIcon::On
    );

    // --- Initiative number ---
    painter->setFont(StyleRepository::textFont(14, true));
    painter->setPen(ColorRepository::selectedForeground());
    painter->drawText(
        cLayout->initiativeRect,
        Qt::AlignVCenter | Qt::AlignLeft,
        QString::number(character->initiative())
    );

    // --- Misc action text (e.g., " drinks potion ", or reuse character->text()) ---
    painter->setFont(StyleRepository::textFont(14, true));
    painter->setPen(ColorRepository::text());
    painter->drawText(
        cLayout->textRect,
        Qt::AlignVCenter | Qt::AlignLeft,
        character->text()   // you can swap with attack-specific label if needed
    );

    // --- Delete button ---
    PainterUtils::paintIcon(
        painter,
        IconRepository::delete_icon(),
        cLayout->deleteButtonRect,
        false,
        cLayout->deleteButtonRect.contains(localCursor),
        cLayout->deleteButtonRect.contains(localCursor) && isPressed,
        cLayout->deleteButtonRect.height()
    );

    // For now, no dropdowns or extra action icons for AttackAction
    Q_UNUSED(castState)
    Q_UNUSED(isSelectedIndex)
    Q_UNUSED(isExpanded)
}

int MiscActionLayoutEngine::minimumWidth(const std::shared_ptr<Character>& character) const {
    return 300;
}
