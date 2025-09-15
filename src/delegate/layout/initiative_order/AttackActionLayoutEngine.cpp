#include "AttackActionLayoutEngine.h"

#include "PainterUtils.h"
#include "IconRepository.h"
#include "StyleRepository.h"

std::shared_ptr<Layout> AttackActionLayoutEngine::calculate(
                                const QRect& rect,
                                const std::shared_ptr<Character>& character,
                                bool isActiveIndex,
                                bool isExpanded) const {
    AttackActionLayout layout;
    auto spec = character->layoutSpec();

    int x = rect.x();
    int y = rect.y();
    int h = rect.height();

    // hero icon (square)
    int heroW = static_cast<int>(h * spec.heroIconScale);
    layout.heroIconRect = QRect(x, y, heroW, h);
    x += heroW + spec.padding;

    // initiative (fixed width, scaled from font metrics or spec)
    int initW = spec.initiativeWidth;
    layout.initiativeRect = QRect(x, y, initW, h);
    x += initW + spec.padding;

    // delete button (square)
    int delW = static_cast<int>(h * spec.deleteButtonScale);
    int delX = rect.right() - delW;
    layout.deleteButtonRect = QRect(delX, y, delW, h);

    // text gets the space in between
    int textW = delX - x - spec.padding;
    layout.textRect = QRect(x, y, textW, h);

    return std::make_shared<AttackActionLayout>(layout);
}

std::optional<HitCommand> AttackActionLayoutEngine::hitTest(
                        const QModelIndex& index,
                        const std::shared_ptr<Layout>& layout,
                        const std::shared_ptr<Character>& character,
                        const QPoint& cursorPos) {

    auto cLayout = std::static_pointer_cast<AttackActionLayout>(layout);

    // implement hit tests for attack action layout

    return std::nullopt;
}

void AttackActionLayoutEngine::paintLayout(
                    QPainter* painter,
                    const std::shared_ptr<Layout>& layout,
                    const std::shared_ptr<Character>& character,
                    const CastState castState,
                    bool isActiveIndex,
                    bool isExpanded,
                    const QPoint& localCursor) const {
    // Attack action currently has no extras beyond base painting
    // You might later add icons for weapon type, advantage/disadvantage, etc.
    Q_UNUSED(painter)
    Q_UNUSED(layout)
    Q_UNUSED(character)
    Q_UNUSED(castState)
    Q_UNUSED(isActiveIndex)
    Q_UNUSED(isExpanded)
    Q_UNUSED(localCursor)
    return;
}

int AttackActionLayoutEngine::minimumWidth(const std::shared_ptr<Character>& character) const {

    auto spec = character->layoutSpec();
    int h = spec.preferredHeight;

    int heroW = static_cast<int>(h * spec.heroIconScale);
    int initW = spec.initiativeWidth;
    int delW  = static_cast<int>(h * spec.deleteButtonScale);

    // minimum text width: font metrics on "Attack" (or fallback)
    QFontMetrics fm(StyleRepository::labelFont(12, false));
    int textW = fm.horizontalAdvance(QStringLiteral("Attack"));

    // total width
    int total = heroW + spec.padding +
                initW + spec.padding +
                textW + spec.padding +
                delW;

    return total;
}
