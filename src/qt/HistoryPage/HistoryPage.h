#pragma once

#include <QWidget>

namespace Ui { class HistoryPage; }

class HistoryPage : public QWidget
{
    Q_OBJECT

public:
    explicit HistoryPage(QWidget *parent = nullptr);
    ~HistoryPage();

private:
    Ui::HistoryPage *ui;
};
