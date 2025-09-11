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

    QFont labelFont(int pointSize = 12, bool bold = false);
    QFont labelPlaceholderFont(int pointSize = 12);
    QFont progressFont(int pointSize = 12, bool bold = false);
    int progressArcWidth();
    
}