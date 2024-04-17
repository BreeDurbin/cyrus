#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui{ class CampaignPage; }
QT_END_NAMESPACE

class CampaignPage : public QWidget
{
    Q_OBJECT

public:
    explicit CampaignPage(QWidget *parent = nullptr);
    ~CampaignPage();

private:
    Ui::CampaignPage *ui;
};
