#include <QPushButton>
#include <QPainter>
#include <QPushButton>
#include <QPainter>
#include <QStyleOptionButton>

class CyrusButton : public QPushButton {
    Q_OBJECT
public:
    CyrusButton(const QString& text, QWidget* parent = nullptr)
        : QPushButton(text, parent), m_buttonImage(":/control/Push_Button.png") {}

protected:
    void paintEvent(QPaintEvent* event) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

        // --- Draw the button image scaled to widget size ---
        if (!m_buttonImage.isNull()) {
            painter.drawPixmap(rect(), m_buttonImage);
        }

        // --- Handle pressed / hovered states by overlay ---
        /*if (isDown()) {
            painter.fillRect(rect(), QColor(0, 0, 0, 60)); // darken when pressed
        } else if (underMouse()) {
            painter.fillRect(rect(), QColor(255, 255, 255, 40)); // lighten when hovered
        }*/

        // --- Draw text centered on top ---
        painter.setPen(Qt::white);
        painter.drawText(rect(), Qt::AlignCenter, text());

        // --- Draw icon (if set) ---
        if (!icon().isNull()) {
            QSize iconSize = this->iconSize();
            QRect iconRect(
                (width() - iconSize.width()) / 2,
                (height() - iconSize.height()) / 2,
                iconSize.width(),
                iconSize.height()
            );
            icon().paint(&painter, iconRect);
        }
    }

private:
    QPixmap m_buttonImage;
};
