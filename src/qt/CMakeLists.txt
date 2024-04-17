
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Widgets Gui REQUIRED)
qt_standard_project_setup()

set(HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/CyrusDialog/CyrusDialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/CombatPage/CombatPage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/CreaturePage/CreaturePage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/WorldPage/WorldPage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/CampaignPage/CampaignPage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/SettingsPage/SettingsPage.h
)

set(SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/CyrusDialog/CyrusDialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CombatPage/CombatPage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CreaturePage/CreaturePage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/WorldPage/WorldPage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CampaignPage/CampaignPage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SettingsPage/SettingsPage.cpp
)

set(UI_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/CyrusDialog/CyrusDialog.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/CombatPage/CombatPage.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/CreaturePage/CreaturePage.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/WorldPage/WorldPage.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/CampaignPage/CampaignPage.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/SettingsPage/SettingsPage.ui
)

set(RESOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/asset/graphics.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/asset/resources.qrc
)

add_library(
    qtLib
    STATIC
    #headers
    ${HEADER_FILES}
    #ui
    ${UI_FILES}
    #sources
    ${SOURCE_FILES}
    #resources
    ${RESOURCE_FILES}
)

target_link_libraries(
    qtLib
    PUBLIC
    styleLib
    modelLib
    Qt6::Core
    Qt6::Widgets 
    Qt6::Gui
)

target_include_directories(
    qtLib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/CyrusDialog
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/CampaignPage
    ${CMAKE_CURRENT_SOURCE_DIR}/CombatPage
    ${CMAKE_CURRENT_SOURCE_DIR}/CreaturePage
    ${CMAKE_CURRENT_SOURCE_DIR}/SettingsPage
    ${CMAKE_CURRENT_SOURCE_DIR}/WorldPage
)