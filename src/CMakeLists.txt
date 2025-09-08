set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui Sql)

# Shared resources
set(RESOURCE_FILES
    ${CMAKE_SOURCE_DIR}/assets/icon.qrc
    ${CMAKE_SOURCE_DIR}/assets/image.qrc
)
qt_add_resources(RCC_SOURCES ${RESOURCE_FILES})

# Add submodules
add_subdirectory(controller)
add_subdirectory(delegate)
add_subdirectory(model)
add_subdirectory(shared)
add_subdirectory(style)
add_subdirectory(view)
add_subdirectory(ui)

# Build qtLib (all non-UI code)
add_library(qtLib STATIC
    ${RCC_SOURCES}
)

# Build app
add_executable(cyrus
    main.cpp 
)

target_link_libraries(
    cyrus 
    PRIVATE 
    controller
    delegate
    model
    shared
    style
    view
    tomlplusplus::tomlplusplus
)
