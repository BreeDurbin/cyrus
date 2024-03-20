

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 

# commented out because specific boost libraries not picked yet
find_package(Boost 1.83.0 COMPONENTS filesystem) 

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt6 COMPONENTS Widgets REQUIRED)
qt_standard_project_setup()

add_executable(
    cyrus
    mainwindow.cpp
    main.cpp
)

target_link_libraries(
    cyrus 
    PRIVATE 
    Qt6::Widgets 
    ${Boost_LIBRARIES}
)

set_target_properties(
    cyrus 
    PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)