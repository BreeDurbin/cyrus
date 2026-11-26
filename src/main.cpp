#include <iostream>
#include <QApplication>
#include <string>
#include <QTextStream>
#include <QDebug>
#include <QPalette>
#include <QCommandLineParser>
#include <QDir>
#include <QFileInfo>
#include <QTimer>

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

    // Options for scripted and headless runs; with none of them Cyrus runs as usual.
    QCommandLineParser parser;
    parser.setApplicationDescription("Cyrus, an AD&D 2e initiative tracker.");
    parser.addHelpOption();
    const QCommandLineOption workingDirOption("working-dir",
        "Run in <dir>, so rosters are read from and saved to <dir>/cyrus_data/roster.", "dir");
    const QCommandLineOption rosterOption("roster",
        "Load the saved roster <name> on startup.", "name");
    const QCommandLineOption quitAfterOption("quit-after",
        "Quit with exit code 0 after <ms> milliseconds.", "ms");
    const QCommandLineOption screenshotOption("screenshot",
        "Save the window as a PNG to <file> when quitting. Quits as soon as the window "
        "is shown unless --quit-after is given. Exits 1 if the file can't be written.", "file");
    parser.addOptions({workingDirOption, rosterOption, quitAfterOption, screenshotOption});
    parser.process(*a);

    // Resolve before --working-dir changes directory, so a relative path means
    // relative to where cyrus was started.
    const QString screenshotPath = parser.isSet(screenshotOption)
        ? QFileInfo(parser.value(screenshotOption)).absoluteFilePath()
        : QString();

    if (parser.isSet(workingDirOption) && !QDir::setCurrent(parser.value(workingDirOption))) {
        std::cerr << "cyrus: cannot change to directory '"
                  << parser.value(workingDirOption).toStdString() << "'" << std::endl;
        return 2;
    }

    int quitAfterMs = -1;
    if (parser.isSet(quitAfterOption)) {
        bool ok = false;
        quitAfterMs = parser.value(quitAfterOption).toInt(&ok);
        if (!ok || quitAfterMs < 0) {
            std::cerr << "cyrus: --quit-after needs a number of milliseconds >= 0" << std::endl;
            return 2;
        }
    } else if (parser.isSet(screenshotOption)) {
        quitAfterMs = 0;
    }

    CyrusDialog w;

    if (parser.isSet(rosterOption)) {
        w.selectRoster(parser.value(rosterOption));
    }

    w.show();

    if (quitAfterMs >= 0) {
        QTimer::singleShot(quitAfterMs, a, [a, &w, screenshotPath]() {
            if (!screenshotPath.isEmpty() && !w.grab().save(screenshotPath, "PNG")) {
                std::cerr << "cyrus: cannot write screenshot '"
                          << screenshotPath.toStdString() << "'" << std::endl;
                a->exit(1);
                return;
            }
            a->exit(0);
        });
    }

    return a->exec();
}
