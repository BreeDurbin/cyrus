#include "PillStepper.h"
#include <QMouseEvent>
#include <QSize>
#include <QPainter>
#include "IconRepository.h"
#include "ColorRepository.h"
#include "StyleRepository.h"
#include "PainterUtils.h"


PillStepper::PillStepper(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(sizeHint());
}

void PillStepper::setText(const QString& text) {
    if (text_ != text) {
        text_ = text;
        update();
    }
}

void PillStepper::updateCombatSequenceState(Cyrus::CombatSequenceState state){
    QString newState = "";
    switch (state) {
        case Cyrus::CombatSequenceState::NPC_DETERMINATION:
            newState = "NPC Determination";
            break;
        case Cyrus::CombatSequenceState::PLAYER_DETERMINATION:
            newState = "Player Determination";
            break;
        case Cyrus::CombatSequenceState::INITIATIVE:
            newState = "Initiative";
            break;
        case Cyrus::CombatSequenceState::RESOLUTION:
            newState = "Resolution";
            break;
    }
    
    setText(newState);
}

void PillStepper::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int rowHeight = static_cast<int>(height() * 0.8);

    // --- Use largest text for layout ---
    static const QString largestText = "Player Determination";
    int labelWidth = fontMetrics().horizontalAdvance(largestText) + 30;

    // Build rects once per paint using fixed width
    rects_ = Stepper::buildStepperRects(rect(), rowHeight, labelWidth);

    // --- Hover/press state ---
    QPoint localCursor = mapFromGlobal(QCursor::pos());
    //StepperState hover = Stepper::buildHoverState(rects_, localCursor);
    //hover.leftPressed = false;
    //hover.rightPressed = false;

    StepperState hover;

    // --- Paint the stepper ---
    Stepper::paintStepper<QString>(&painter, rects_, hover, text_, false);
}



void PillStepper::mousePressEvent(QMouseEvent* event) {
    if (rects_.leftRect.contains(event->pos())) {
        emit stepBackward();
    } else if (rects_.rightRect.contains(event->pos())) {
        emit stepForward();
    }
}

QSize PillStepper::sizeHint() const {
    static const QString largestText = "Player Determination";

    QFontMetrics fm(font());

    // Text width
    int textWidth = fm.horizontalAdvance(largestText);

    // Arrow size (match how Stepper::buildStepperRects scales arrows)
    // Suppose buildStepperRects uses rowHeight * 0.5 for arrows
    int textHeight = fm.height();
    double arrowScale = 0.5; // same as buildStepperRects
    int arrowWidth = static_cast<int>(textHeight / arrowScale); // ensure enough room

    int horizontalPadding = 20; // extra space on left/right

    int w = textWidth + (2 * arrowWidth) + horizontalPadding;
    int h = textHeight + 20; // vertical padding for arrows and text

    return QSize(w, h);
}
