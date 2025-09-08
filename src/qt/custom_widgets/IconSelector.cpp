// IconSelector.cpp
#include "IconSelector.h"
#include <QGraphicsDropShadowEffect>

IconSelector::IconSelector(QWidget* parent, QList<QIcon> icons) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(10);

    buttonGroup_ = new QButtonGroup(this);
    buttonGroup_->setExclusive(true);

    for (int i = 0; i < icons.size(); ++i) {
        auto* btn = new QPushButton(this);
        btn->setCheckable(true);
        btn->setIcon(icons[i]);
        btn->setIconSize(QSize(48, 48));
        btn->setFlat(true);  // hide borders

        // Optional glow effect when checked
        auto* glow = new QGraphicsDropShadowEffect(btn);
        glow->setBlurRadius(20);
        glow->setColor(Qt::yellow);
        glow->setOffset(0);
        btn->setGraphicsEffect(glow);
        glow->setEnabled(false);

        connect(btn, &QPushButton::toggled, this, [glow](bool checked){
            glow->setEnabled(checked);
        });

        buttonGroup_->addButton(btn, i);
        layout->addWidget(btn);

        //default to first icon selected
        if (i == 0) {
            btn->setChecked(true);
        }
    }

    connect(buttonGroup_, &QButtonGroup::buttonClicked,
        this, [this](QAbstractButton* button) {
            int id = buttonGroup_->id(button);
            emit iconSelected(id);
        });
        
}

int IconSelector::currentIndex() const {
    return buttonGroup_->checkedId();
}
