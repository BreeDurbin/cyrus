
#include <iostream>
#include <QApplication>
#include <string>
#include <QTextStream>
#include <QDebug>
#include <QPalette>

#include <CyrusDialog.h>

int main(int argc, char* argv[])
{   

    qRegisterMetaType<std::shared_ptr<Character>>("std::shared_ptr<Character>");
    qRegisterMetaType<std::weak_ptr<Character>>("std::weak_ptr<Character>");
    
    QApplication *a = new QApplication(argc, argv);
    QGuiApplication::setDesktopFileName("org.cyrus.Cyrus");
    
    CyrusDialog w;

    w.show();

    return a->exec();
}
