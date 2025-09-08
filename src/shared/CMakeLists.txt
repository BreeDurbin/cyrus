add_library(shared INTERFACE)

# Expose includes
target_include_directories(shared
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Expose dependencies (Qt6 everywhere)
target_link_libraries(shared
    INTERFACE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
)
