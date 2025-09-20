set(VIEW_HEADERS
    InitiativeView.h
    custom_widgets/IconSelector.h
    custom_widgets/PillStepper.h
    custom_widgets/Stepper.h
    custom_widgets/CyrusButton.h
)

set(VIEW_SOURCES
    InitiativeView.cpp
    custom_widgets/IconSelector.cpp
    custom_widgets/PillStepper.cpp
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
    ${CMAKE_CURRENT_SOURCE_DIR}/custom_widgets
)