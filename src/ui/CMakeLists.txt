set(UI_HEADERS
    CyrusDialog.h
)

set(UI_SOURCES
    CyrusDialog.cpp
    CyrusDialog.ui
)

add_library(
    ui 
    STATIC
    ${UI_HEADERS}
    ${UI_SOURCES}
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

set_target_properties(ui PROPERTIES AUTOMOC ON AUTOUIC ON AUTORCC ON)
