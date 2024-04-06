
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
    ${CMAKE_CURRENT_SOURCE_DIR}/component/include/cpushbutton.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/component/include/ccombobox.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/component/include/clistview.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/component/include/ctabwidget.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/component/include/clabel.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/stylesheet/dracula.h
)

set(SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/component/cpushbutton.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/component/ccombobox.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/component/clistview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/component/ctabwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/component/clabel.cpp
)

add_library(
    styleLib
    STATIC
    ${HEADER_FILES}
    ${SOURCE_FILES}
)

target_include_directories(
    styleLib
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/component/include
    ${CMAKE_CURRENT_SOURCE_DIR}/stylesheet
    ${Boost_INCLUDE_DIRS}
)

target_link_libraries(
    styleLib
    PRIVATE 
    Qt6::Core
    Qt6::Widgets 
    Qt6::Gui
    ${Boost_LIBRARIES}
)