

add_library(shared STATIC
    Enums.h
    custom_widgets/
)

# shared is the Qt hub
target_link_libraries(
    shared
    PUBLIC 
    Qt6::Core 
    Qt6::Gui 
    Qt6::Widgets
)

target_include_directories(
    shared
    PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/custom_widgets
)
