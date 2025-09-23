set(VIEW_HEADERS
    RosterView.h
    InitiativeView.h
    custom_widgets/IconSelector.h
    custom_widgets/PillStepper.h
    custom_widgets/Stepper.h
    custom_widgets/CyrusButton.h
    custom_widgets/CyrusComboBox.h
    custom_widgets/CyrusToolButton.h
)

set(VIEW_SOURCES
    RosterView.cpp
    InitiativeView.cpp
    custom_widgets/IconSelector.cpp
    custom_widgets/PillStepper.cpp
    custom_widgets/CyrusComboBox.cpp
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