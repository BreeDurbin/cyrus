
#include <iostream>
#include <QApplication>
#include <string>
#include <QTextStream>
#include <QDebug>
#include <QPalette>
#include "mainwindow.h"

int main(int argc, char* argv[])
{
    QApplication *a = new QApplication(argc, argv);

    std::cout << "Entering qt event loop" << std::endl;

    QPalette pal = a->palette();
    QColor graphite(39,44,50);
    pal.setColor(QPalette::Window, graphite);
    a->setPalette(pal);

    MainWindow w;
    w.show();

    return a->exec();
}
