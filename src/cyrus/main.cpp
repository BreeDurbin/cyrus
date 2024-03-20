
#include <iostream>
#include <QApplication>
#include <string>
#include <QTextStream>
#include <QDebug>
#include "mainwindow.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    std::cout << "Entering qt event loop" << std::endl;

    MainWindow w;
    w.show();

    return a.exec();
}
