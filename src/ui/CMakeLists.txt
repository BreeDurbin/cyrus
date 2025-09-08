# src/ui/CMakeLists.txt

set(UI_HEADERS
    CyrusDialog.h
)

set(UI_SOURCES
    CyrusDialog.cpp
)

set(UI_FORMS
    CyrusDialog.ui
)

qt_wrap_ui(UI_SOURCES ${UI_FORMS})

add_library(
    ui 
    STATIC
    ${UI_HEADERS}
    ${UI_SOURCES}
    TomlHelper.cpp
    TomlHelper.h
)

target_link_libraries(
    ui
    PRIVATE
    shared
    controller
    view
    delegate
    style
    model
)

target_include_directories(
    ui 
    PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)
