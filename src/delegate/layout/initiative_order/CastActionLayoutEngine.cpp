#include "CastActionLayoutEngine.h"

#include "LayoutUtils.h"
#include "IconRepository.h"
#include "StyleRepository.h"
#include "ColorRepository.h"
#include "PainterUtils.h"
#include "LayoutUtils.h"
#include "CastAction.h"
#include <QApplication>
#include <QPainter>


std::shared_ptr<Layout> CastActionLayoutEngine::calculate(
    const QRect& rect,
    const std::shared_ptr<Character>& character,
    bool isSelectedIndex,
    bool isExpanded,
    const Cyrus::CombatSequenceState state
) const {

    CastActionLayout layout;
    auto spec = character->layoutSpec();
    int padding = spec.padding;

    // --- base + main row ---
    layout.baseRect     = LayoutUtils::padRectangle(rect, padding); //unpadded, used to build the top and bottom row rects
    layout.mainRowRect  = LayoutUtils::buildMainRowRect(rect, false, padding);

    // --- left-hand controls (hero + initiative + duration icon + duration text) ---
    LayoutUtils::LeftAnchorBuilder left(layout.mainRowRect);

    layout.heroIconRect       = left.icon(spec.heroIconScale, padding);
    layout.initiativeRect     = left.text(spec.initiativeWidth, padding);

    layout.durationIconRect = left.icon(spec.durationIconScale, padding);

    int durationTextWidth = 50; // you can tweak or compute based on font metrics
    layout.durationTextRect = left.text(durationTextWidth, padding);

    // --- right-hand control (delete button) ---
    LayoutUtils::RightAnchorBuilder right(layout.mainRowRect);
    layout.deleteButtonRect = right.icon(spec.deleteButtonScale, padding);

    // --- main text (fills remaining space between last left rect and delete button) ---
    int textWidth = layout.deleteButtonRect.left() - left.cursorX - padding;

    layout.textRect = left.text(textWidth, padding);

    Q_UNUSED(isExpanded)

    return std::make_shared<CastActionLayout>(layout);
}


std::optional<HitCommand> CastActionLayoutEngine::hitTest(
    const QModelIndex& index,
    const std::shared_ptr<Layout>& layout,
    const std::shared_ptr<Character>& character,
    const QPoint& cursorPos,
    const Cyrus::CombatSequenceState state
) 
{
    auto cLayout = std::static_pointer_cast<CastActionLayout>(layout);
    const QUuid id = character->uuid();

    // Only delete button is clickable
    if (cLayout->deleteButtonRect.contains(cursorPos)) {
        return HitCommand{ [index, this](QListView* /*view*/) {
            qDebug() << "Delete button clicked for cast action at row:" << index.row();
            emit deleteItemClicked(index);
        }};
    }

    return std::nullopt; // nothing else clickable
}


#include <QDebug>

void CastActionLayoutEngine::paintLayout(
    QPainter* painter,
    const std::shared_ptr<Layout>& layout,
    const std::shared_ptr<Character>& character,
    const CastState castState,
    bool isSelectedIndex,
    bool isExpanded,
    const QPoint& localCursor,
    const Cyrus::CombatSequenceState state
) const 
{
    auto cLayout = std::static_pointer_cast<CastActionLayout>(layout);
    auto spec = character->layoutSpec();
    bool isPressed = QApplication::mouseButtons() & Qt::LeftButton;
    auto castAction = std::static_pointer_cast<CastAction>(character);

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

    // --- Duration icon ---
    QIcon durationIcon = IconRepository::hourglass_icon();
    durationIcon.paint(
        painter,
        cLayout->durationIconRect,
        Qt::AlignCenter,
        QIcon::Normal,
        QIcon::On
    );

    // --- Duration text ---
    painter->setFont(StyleRepository::textFont(12, false));
    painter->setPen(ColorRepository::text());
    painter->drawText(
        cLayout->durationTextRect,
        Qt::AlignVCenter | Qt::AlignLeft,
        QString::number(castAction->duration())
    );

    // --- Cast spell text ---
    painter->setFont(StyleRepository::textFont(14, true));
    painter->setPen(ColorRepository::text());
    painter->drawText(
        cLayout->textRect,
        Qt::AlignVCenter | Qt::AlignLeft,
        castAction->text()
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

    Q_UNUSED(castState)
    Q_UNUSED(isSelectedIndex)
    Q_UNUSED(isExpanded)
}


int CastActionLayoutEngine::minimumWidth(const std::shared_ptr<Character>& character) const {
    auto spec = character->layoutSpec();
    int h = spec.preferredHeight;

    // Hero icon width
    int heroW = static_cast<int>(h * spec.heroIconScale);

    // Initiative width
    int initW = spec.initiativeWidth;

    // Duration icon width
    int durationIconW = static_cast<int>(h * spec.durationIconScale);

    // Duration text width (estimate using font metrics)
    QFontMetrics fm(StyleRepository::textFont(12, false));
    int durationTextW = fm.horizontalAdvance(QStringLiteral("99")); // max 2-digit duration

    // Delete button width
    int delW = static_cast<int>(h * spec.deleteButtonScale);

    // Minimum width for cast spell text
    int textW = fm.horizontalAdvance(QStringLiteral("Spell name…"));

    // Total width including paddings
    int total = heroW + spec.padding +
                initW + spec.padding +
                durationIconW + spec.padding +
                durationTextW + spec.padding +
                textW + spec.padding +
                delW;

    return total;
}

