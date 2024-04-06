
#include <iostream>
#include <QApplication>
#include <string>
#include <QTextStream>
#include <QDebug>
#include <QPalette>

#include <mainwindow.hpp>

int main(int argc, char* argv[])
{      
    QApplication *a = new QApplication(argc, argv);

    MainWindow w;
    w.show();

    return a->exec();
}
