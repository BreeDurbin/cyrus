#pragma once

#include<QString>
#include<QFont>
#include <QStringList>


namespace StyleRepository
{
    QString hero();
    QString mainWindow();
    QString label();
    QString itemView();
    QString pushButton();
    QString iconPushButton();
    QString spinBox();
    QString lineEdit();
    QString comboBox();

    QString titleFontFamily();
    QFont titleFont(int pointSize = 12, bool bold = false);
    QString textFontFamily();
    QFont textFont(int pointSize = 12, bool bold = false);
    int progressArcWidth();
    
}