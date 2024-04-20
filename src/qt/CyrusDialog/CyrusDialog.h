#pragma once

#include <QMainWindow>
#include <QVBoxLayout>

namespace Ui { class CyrusDialog; }

class CyrusDialog 
    : public QMainWindow 
{
    Q_OBJECT

public:
    CyrusDialog(QWidget *parent = nullptr);
    ~CyrusDialog();

private:
    void addCombatPage();
    void addCreaturePage();
    void addWorldPage();
    void addHistoryPage();
    void addCampaignPage();
    void addSettingsPage();

protected:
    Ui::CyrusDialog *ui;

};
