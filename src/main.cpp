
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
    
#ifdef CYRUS_STANDALONE
    // The standalone Qt has no xcb/wayland plugin; default to offscreen unless
    // the caller picked a platform (e.g. QT_QPA_PLATFORM=vnc).
    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM")) {
        qputenv("QT_QPA_PLATFORM", "offscreen");
    }
    // Without fontconfig Qt looks for fonts in <qt>/lib/fonts, which doesn't exist.
    if (qEnvironmentVariableIsEmpty("QT_QPA_FONTDIR")) {
        qputenv("QT_QPA_FONTDIR", CYRUS_FONT_DIR);
    }
#endif

    QApplication *a = new QApplication(argc, argv);
    QGuiApplication::setDesktopFileName("org.cyrus.Cyrus");
    
    CyrusDialog w;

    w.show();

    return a->exec();
}
