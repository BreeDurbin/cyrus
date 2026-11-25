# ---- Catch2 (vendored amalgamated build) ----
set(CATCH2_DIR ${CMAKE_SOURCE_DIR}/third_party/catch2-3.16.1)

add_library(catch2 STATIC ${CATCH2_DIR}/catch_amalgamated.cpp)
target_include_directories(catch2 PUBLIC ${CATCH2_DIR})
# main() lives in tests/main.cpp so it can create the QApplication first
target_compile_definitions(catch2 PUBLIC CATCH_AMALGAMATED_CUSTOM_MAIN)

# ---- Unit tests: model and controller layers only ----
add_executable(
    cyrus_tests
    main.cpp
    TestHelpers.h
    model/AttackRateTests.cpp
    model/CharacterTests.cpp
    model/ActionTests.cpp
    model/CharacterModelTests.cpp
    model/RosterModelTests.cpp
    controller/TomlHelperTests.cpp
    controller/CharacterControllerTests.cpp
)

target_include_directories(cyrus_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(
    cyrus_tests
    PRIVATE
        CYRUS_GOLD_DATA_DIR="${CMAKE_SOURCE_DIR}/gold_data/cyrus_data/roster"
)

target_link_libraries(
    cyrus_tests
    PRIVATE
        catch2
        controller
        model
        shared
        Qt6::Widgets
)

# Register each TEST_CASE with ctest. PRE_TEST lists the tests when ctest runs
# instead of at build time.
list(APPEND CMAKE_MODULE_PATH ${CATCH2_DIR})
include(Catch)
catch_discover_tests(
    cyrus_tests
    DISCOVERY_MODE PRE_TEST
    PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)
