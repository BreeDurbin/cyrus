
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Widgets Gui REQUIRED)
qt_standard_project_setup()

add_executable(
    ${PROJECT_NAME}
    #sources
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_link_libraries(
    ${PROJECT_NAME} 
    PRIVATE
    qtLib
)

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)
