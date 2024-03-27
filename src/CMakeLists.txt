
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 

find_package(Boost 1.83.0 COMPONENTS filesystem) 

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt6 COMPONENTS Core Widgets Gui REQUIRED)
qt_standard_project_setup()


add_executable(
    ${PROJECT_NAME}
    #src
    mainwindow.cpp
    main.cpp

    #component
    #creaturecombobox.cpp
    #creaturerepository.cpp
    #effectrepository.cpp

    #model
    #effectmodel.cpp
    model/creaturemodel.cpp

    #stylesheet
    stylesheet/dracula.h

    #resources
    resources.qrc
)

target_link_libraries(
    ${PROJECT_NAME} 
    PRIVATE 
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