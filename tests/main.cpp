#include <catch_amalgamated.hpp>

#include <QApplication>

#include "Character.h"

namespace {

// Character keeps a global name -> count cache for numbering ("Orc 1", "Orc 2"),
// so reset it every time a test case body runs (each SECTION / GENERATE run
// re-enters the body) to keep runs independent.
class NumberCacheReset : public Catch::EventListenerBase {
public:
    using Catch::EventListenerBase::EventListenerBase;

    void testCasePartialStarting(const Catch::TestCaseInfo&, uint64_t) override {
        Character::clearNumberCache();
    }
};

}

CATCH_REGISTER_LISTENER(NumberCacheReset)

int main(int argc, char* argv[])
{
    // Models, icons and InitiativeView need a QApplication; tests never need a display.
    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM")) {
        qputenv("QT_QPA_PLATFORM", "offscreen");
    }
    // Keep test output readable; the code under test logs heavily via qDebug.
    qputenv("QT_LOGGING_RULES", "*.debug=false");

    qRegisterMetaType<std::shared_ptr<Character>>("std::shared_ptr<Character>");
    qRegisterMetaType<std::weak_ptr<Character>>("std::weak_ptr<Character>");

    QApplication app(argc, argv);

    return Catch::Session().run(argc, argv);
}
