#include <memory>
#include "Character.h"
#include "LayoutUtils.h"

class CombatLogLayoutEngine {

    public:

    // preferred width accounting for wrap at given max text width
    static int minimumWidth(const std::shared_ptr<Character>& character, int maxTextWidth = 300) // optional wrap width
    {
        auto spec = character->layoutSpec();
        int padding = spec.padding;
        return padding + (spec.heroIconScale * spec.preferredHeight) + padding + maxTextWidth + padding;
    }

};

