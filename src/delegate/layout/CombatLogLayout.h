#include <memory>
#include "Character.h"

class CombatLogLayout{

    public:

    // preferred width accounting for wrap at given max text width
    static int preferredWidth(const Character::LayoutSpec& spec,
                              const std::shared_ptr<Character>& character,
                              int maxTextWidth = 300) // optional wrap width
    {
        // Base rect with arbitrary height
        QRect baseRect(0, 0, 1000, spec.preferredHeight);

        // Hero icon
        const int defaultIconWidth = 32; // or spec.defaultIconHeight
        QRect heroIconRect(baseRect.left(), baseRect.top(), defaultIconWidth, defaultIconWidth);

        // Right boundary (dummy rect)
        QRect rightBoundary(baseRect.right(), baseRect.top(), 0, baseRect.height());

        // Text rect (everything between icon and right boundary)
        QRect textRect = LayoutUtils::buildTextRect(heroIconRect, rightBoundary, spec.padding);

        // Use QFontMetrics boundingRect to measure wrapped text
        QFontMetrics fm(QApplication::font());
        QRect bounding = fm.boundingRect(QRect(0, 0, maxTextWidth, INT_MAX),
                                         Qt::AlignLeft | Qt::TextWordWrap,
                                         character->combatLog());

        int textWidth = bounding.width(); // width of wrapped text

        // Total width = hero icon + padding + text width + padding
        int totalWidth = heroIconRect.width() + spec.padding + textWidth + spec.padding;

        return totalWidth;
    }

};

