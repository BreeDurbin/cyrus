
#include <QProxyStyle>
#include <memory>

class PushButtonStyleHelper;

// QProxyStyle to see all widgets in an application (even if it means mixing with the default style)
// QCommonStyle to only see what we have styled, no bad interactions from the default style, but the other widgets won't be usable
using super = QCommonStyle;

class CStyle : public super
{
public:
    CStyle();

    void drawPrimitive(PrimitiveElement pe, const QStyleOption *option, QPainter *painter,
                       const QWidget *widget = nullptr) const override;

    int pixelMetric(PixelMetric pm, const QStyleOption *option = nullptr,
                    const QWidget *widget = nullptr) const override;

    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter,
                     const QWidget *widget = nullptr) const override;

    void drawComplexControl(ComplexControl complexControl, const QStyleOptionComplex *opt, QPainter *painter,
                            const QWidget *widget = nullptr) const override;

    int styleHint(StyleHint stylehint, const QStyleOption *option, const QWidget *widget, QStyleHintReturn *returnData) const override;

    QSize sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &contentsSize, const QWidget *widget) const override;

    QRect subElementRect(SubElement subElement, const QStyleOption *option, const QWidget *widget) const override;

    void polish(QWidget *w) override;

    bool eventFilter(QObject *obj, QEvent *event) override;


    PushButtonStyleHelper* pushButtonStyleHelper() const;

private:
    std::unique_ptr<class PushButtonStyleHelper> mPushButtonStyleHelper;

};