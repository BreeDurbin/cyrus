set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Add submodules
add_subdirectory(controller)
add_subdirectory(delegate)
add_subdirectory(model)
add_subdirectory(shared)
add_subdirectory(style)
add_subdirectory(view)
add_subdirectory(ui)

# Build app
add_executable(
    cyrus
    main.cpp
)

# Link everything
target_link_libraries(
    cyrus
    PRIVATE
        ui
        view
        delegate
        controller
        model
        shared
        style
        tomlplusplus::tomlplusplus
        asset
)


