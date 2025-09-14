#pragma once

struct StepperRects {
    QRect minusRect;
    QRect valueRect;
    QRect plusRect;
};

class Stepper {
public:
    inline static StepperRects buildStepperRects(const QRect& frame,
                                                 int buttonW,
                                                 int valueW,
                                                 int h,
                                                 int padding) 
    {
        StepperRects s;

        // shorter button height (e.g. 80% of value height)
        int buttonH = static_cast<int>(h * 0.8);
        int y = frame.top() + (frame.height() - h) / 2;

        int x = frame.left();

        // minus button (centered vertically against full stepper height)
        s.minusRect = QRect(x,
                            y + (h - buttonH) / 2,
                            buttonW,
                            buttonH);
        x += buttonW + padding;

        // value box (full height)
        s.valueRect = QRect(x, y, valueW, h);
        x += valueW + padding;

        // plus button (same as minus)
        s.plusRect = QRect(x,
                           y + (h - buttonH) / 2,
                           buttonW,
                           buttonH);

        return s;
    }

    
};
