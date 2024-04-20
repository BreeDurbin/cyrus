
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Widgets Gui REQUIRED)
qt_standard_project_setup()

set(HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Creature/Creature.h
    ${CMAKE_CURRENT_SOURCE_DIR}/CreatureModel/CreatureModel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/CreatureEditor/CreatureEditor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/CreatureDelegate/CreatureDelegate.h
)

set(SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Creature/Creature.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CreatureModel/CreatureModel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CreatureEditor/CreatureEditor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CreatureDelegate/CreatureDelegate.cpp
)

add_library(
    modelLib
    #sources
    ${SOURCE_FILES}
)

target_link_libraries(
    modelLib
    PRIVATE
    styleLib
    Qt6::Core
    Qt6::Widgets 
    Qt6::Gui
)

target_include_directories(
    modelLib
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/Creature
    ${CMAKE_CURRENT_SOURCE_DIR}/CreatureModel
    ${CMAKE_CURRENT_SOURCE_DIR}/CreatureEditor
    ${CMAKE_CURRENT_SOURCE_DIR}/CreatureDelegate
)