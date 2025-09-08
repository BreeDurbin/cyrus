set(VIEW_HEADERS
    InitiativeView.h
    IconSelector.h
)

set(VIEW_SOURCES
    InitiativeView.cpp
    IconSelector.cpp
)

add_library( 
    view 
    STATIC 
    ${VIEW_HEADERS} 
    ${VIEW_SOURCES}
)

target_link_libraries(
    view
    PUBLIC
    shared
    model
    delegate
    style
)

target_include_directories(
    view 
    PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)