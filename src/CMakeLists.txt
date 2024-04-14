
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 

find_package(Boost 1.83.0 COMPONENTS filesystem) 

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Widgets Gui REQUIRED)
qt_standard_project_setup()

set(HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mainwindow.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/model/include/creaturemodel.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.ui
)

set(SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/model/creaturemodel.cpp
)

set(RESOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/graphics.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc
)


add_executable(
    ${PROJECT_NAME}
    #headers
    ${HEADER_FILES}
    #sources
    ${SOURCE_FILES}
    #resources
    ${RESOURCE_FILES}
)

target_include_directories(
    ${PROJECT_NAME} 
    PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/component/include
    ${CMAKE_CURRENT_SOURCE_DIR}/model/include
)

target_link_libraries(
    ${PROJECT_NAME} 
    PRIVATE 
    styleLib
    Qt6::Core
    Qt6::Widgets 
    Qt6::Gui
    ${Boost_LIBRARIES}
)

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)
