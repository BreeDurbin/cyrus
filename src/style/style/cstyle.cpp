
#include "include/cstyle.h"
#include "include/pushbuttonstylehelper.h"

#include <QPushButton>
#include <QStyleOptionButton>

CStyle::CStyle()
    : super()
    , mPushButtonStyleHelper(new PushButtonStyleHelper)
{
}

void CStyle::drawPrimitive(PrimitiveElement pe, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    switch (pe) {
    case PE_FrameFocusRect:
        // nothing, we don't want focus rects
        break;
    default:
        super::drawPrimitive(pe, option, painter, widget);
        break;
    }
}

int CStyle::pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
    switch (metric) {
    case PM_ButtonShiftHorizontal:
    case PM_ButtonShiftVertical:
        return 0; // no shift

    default:
        return super::pixelMetric(metric, option, widget);
    }
}

void CStyle::drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    switch (element) {
    case CE_PushButton:
        // The default implementation calls CE_PushButtonBevel, CE_PushButtonLabel
        // and PE_FrameFocusRect (which we reimplement to nothing).
        QCommonStyle::drawControl(element, option, painter, widget);
        return;
    case CE_PushButtonBevel:
        // Draw button shape (background and border). This one we'll fully implement ourselves.
        if (const auto *optionButton = qstyleoption_cast<const QStyleOptionButton *>(option)) {
            mPushButtonStyleHelper->drawButtonShape(optionButton, painter, widget);
        }
        return;
    case CE_PushButtonLabel:
        // Draw button text, icon (and menu indicator)
        // We just want to call the base class, with an adjusted palette
        if (const auto *optionButton = qstyleoption_cast<const QStyleOptionButton *>(option)) {
            QStyleOptionButton copy = *optionButton;
            mPushButtonStyleHelper->adjustTextPalette(&copy);
            QCommonStyle::drawControl(element, &copy, painter, widget);
        }
        return;

    case CE_RadioButton: // simply calls PE_IndicatorRadioButton, CE_RadioButtonLabel (and focus rect)
    case CE_RadioButtonLabel:
    default:
        super::drawControl(element, option, painter, widget);
    }
}

void CStyle::polish(QWidget *w)
{
    if (qobject_cast<QPushButton *>(w)) {
        w->setAttribute(Qt::WA_Hover);
    }
    super::polish(w);
}

bool CStyle::eventFilter(QObject *obj, QEvent *event)
{
    return super::eventFilter(obj, event);
}

PushButtonStyleHelper *CStyle::pushButtonStyleHelper() const
{
    return mPushButtonStyleHelper.get();
}

int CStyle::styleHint(StyleHint stylehint, const QStyleOption *option, const QWidget *widget,
                         QStyleHintReturn *returnData) const
{
    switch (stylehint) {
    case SH_DialogButtonBox_ButtonsHaveIcons:
        return 0;
    default:
        break;
    }

    return super::styleHint(stylehint, option, widget, returnData);
}

QSize CStyle::sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &contentsSize,
                                  const QWidget *widget) const
{
    switch (type) {
    case CT_PushButton:
        if (const auto *buttonOption = qstyleoption_cast<const QStyleOptionButton *>(option)) {
            return mPushButtonStyleHelper->sizeFromContents(buttonOption, contentsSize, widget);
        }
        break;
    case CT_RadioButton:
    case CT_CheckBox:
        return QCommonStyle::sizeFromContents(type, option, contentsSize, widget);

    default:
        break;
    }
    return super::sizeFromContents(type, option, contentsSize, widget);
}

QRect CStyle::subElementRect(SubElement subElement, const QStyleOption *option, const QWidget *widget) const
{
    switch (subElement) {
    default:
        break;
    }
    return super::subElementRect(subElement, option, widget);
}

void CStyle::drawComplexControl(ComplexControl complexControl, const QStyleOptionComplex *option, QPainter *painter,
                                   const QWidget *widget) const
{
    switch (complexControl) {
    default:
        super::drawComplexControl(complexControl, option, painter, widget);
    }
}