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

if(CYRUS_STANDALONE)
    # The vendored Qt has no fontconfig, so point it at our bundled fonts.
    target_compile_definitions(
        cyrus
        PRIVATE
            CYRUS_STANDALONE
            CYRUS_FONT_DIR="${CMAKE_SOURCE_DIR}/asset/font"
    )
endif()


