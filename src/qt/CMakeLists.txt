
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Widgets Gui Sql REQUIRED)
qt_standard_project_setup()

set(HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/CyrusDialog/CyrusDialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/CombatPage/CombatPage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/CreaturePage/CreaturePage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/WorldPage/WorldPage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/HistoryPage/HistoryPage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/CampaignPage/CampaignPage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/SettingsPage/SettingsPage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Database/Database.h
)

set(SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/CyrusDialog/CyrusDialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CombatPage/CombatPage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CreaturePage/CreaturePage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/WorldPage/WorldPage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/HistoryPage/HistoryPage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CampaignPage/CampaignPage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SettingsPage/SettingsPage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Database/Database.cpp
)

set(UI_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/CyrusDialog/CyrusDialog.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/CombatPage/CombatPage.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/CreaturePage/CreaturePage.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/WorldPage/WorldPage.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/HistoryPage/HistoryPage.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/CampaignPage/CampaignPage.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/SettingsPage/SettingsPage.ui
)

set(RESOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/asset/icon.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/asset/image.qrc
)

qt_add_resources (RCC_SOURCES ${RESOURCE_FILES})


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
    ${RCC_SOURCES}
)

target_link_libraries(
    qtLib
    PUBLIC
    styleLib
    modelLib
    Qt6::Core
    Qt6::Widgets 
    Qt6::Gui
    Qt6::Sql
)

target_include_directories(
    qtLib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/CyrusDialog
    ${CMAKE_CURRENT_SOURCE_DIR}/asset
    ${CMAKE_CURRENT_SOURCE_DIR}/asset/icon
    ${CMAKE_CURRENT_SOURCE_DIR}/asset/image
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/CampaignPage
    ${CMAKE_CURRENT_SOURCE_DIR}/CombatPage
    ${CMAKE_CURRENT_SOURCE_DIR}/CreaturePage
    ${CMAKE_CURRENT_SOURCE_DIR}/SettingsPage
    ${CMAKE_CURRENT_SOURCE_DIR}/WorldPage
    ${CMAKE_CURRENT_SOURCE_DIR}/HistoryPage
)