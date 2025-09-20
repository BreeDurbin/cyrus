#pragma once
#include <QWidget>
#include <QString>
#include "Stepper.h"

class PillStepper : public QWidget {
    Q_OBJECT
public:
    explicit PillStepper(QWidget* parent = nullptr);

    void setText(const QString& text);
    QString text() const { return text_; }

signals:
    void stepForward();
    void stepBackward();

public slots:
    void updateCombatSequenceState(Cyrus::CombatSequenceState state);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    QSize sizeHint() const override;

private:
    QString text_ = "NPC Determination";
    StepperRects rects_;
};
