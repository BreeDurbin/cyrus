#pragma once

#include <QWidget>

namespace Ui { class WorldPage; }

class WorldPage : public QWidget
{
    Q_OBJECT

public:
    explicit WorldPage(QWidget *parent = nullptr);
    ~WorldPage();

private:
    Ui::WorldPage *ui;
};
