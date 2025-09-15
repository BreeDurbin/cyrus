#include "CastActionLayoutEngine.h"


std::shared_ptr<Layout> CastActionLayoutEngine::calculate(
                const QRect& rect,
                const std::shared_ptr<Character>& character,
                bool isActiveIndex,
                bool isExpanded) const {
    auto spec = character->layoutSpec();
    return nullptr;
}

std::optional<HitCommand> CastActionLayoutEngine::hitTest(
                            const QModelIndex& index,
                            const std::shared_ptr<Layout>& layout,
                            const std::shared_ptr<Character>& character,
                            const QPoint& cursorPos) {
    
    auto cLayout = std::static_pointer_cast<CastActionLayout>(layout);
    // implement hit tests for cast action layout
    return std::nullopt;
}

void CastActionLayoutEngine::paintLayout(
                    QPainter* painter,
                    const std::shared_ptr<Layout>& layout,
                    const std::shared_ptr<Character>& character,
                    const CastState castState,
                    bool isActiveIndex,
                    bool isExpanded,
                    const QPoint& localCursor ) const {
    return;
}

int CastActionLayoutEngine::minimumWidth(const std::shared_ptr<Character>& character) const {
    return 0;
}
