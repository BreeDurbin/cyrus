// IconSelector.h
#pragma once

#include <QWidget>
#include <QPushButton>
#include <QButtonGroup>
#include <QHBoxLayout>
#include "../style/IconRepository.h"

class IconSelector : public QWidget {
    Q_OBJECT
public:
    explicit IconSelector(QWidget* parent = nullptr, QList<QIcon> icons = IconRepository::classIcons());

    int currentIndex() const;

signals:
    void iconSelected(int index);

private:
    QButtonGroup* buttonGroup_;
};
