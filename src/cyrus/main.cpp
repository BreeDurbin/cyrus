
#include <iostream>
#include <QCoreApplication>
#include <string>
#include <QTextStream>
#include <QDebug>

void do_cpp(){
    std::string name;

    std::cout << "cpp way - Please enter your name: " << std::endl;
    std::getline(std::cin, name);
    std::cout << "cpp out Hello: " << name << std::endl;
}

void do_qt(){
    QTextStream qin(stdin);
    QTextStream qout(stdout);

    qout << "qt way - Please enter your name: "; 
    qout.flush();
    QString name = qin.readLine();
    qout << "qout Hello: " << name << "\n";
    qout.flush();
}

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);

    std::cout << "Entering qt event loop" << std::endl;

    do_cpp();

    do_qt();

    return a.exec();
}
