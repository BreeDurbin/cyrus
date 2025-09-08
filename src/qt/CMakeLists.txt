
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Widgets Gui Sql REQUIRED)
qt_standard_project_setup()

set(HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/cyrusdialog/CyrusDialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/model/Character.h
    ${CMAKE_CURRENT_SOURCE_DIR}/model/CharacterAction.h
    ${CMAKE_CURRENT_SOURCE_DIR}/model/CharacterModel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/model/InitiativeDelegate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/model/CombatLogDelegate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/style/ColorRepository.h
    ${CMAKE_CURRENT_SOURCE_DIR}/style/StyleRepository.h
    ${CMAKE_CURRENT_SOURCE_DIR}/style/IconRepository.h
    ${CMAKE_CURRENT_SOURCE_DIR}/custom_widgets/IconSelector.h
    ${CMAKE_CURRENT_SOURCE_DIR}/helper/TomlHelper.h
    ${CMAKE_CURRENT_SOURCE_DIR}/helper/RectBuilder.h
    ${CMAKE_CURRENT_SOURCE_DIR}/helper/Enums.h
    ${CMAKE_CURRENT_SOURCE_DIR}/view/InitiativeView.h
)

set(SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/cyrusdialog/CyrusDialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/model/Character.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/model/CharacterAction.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/model/CharacterModel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/model/InitiativeDelegate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/model/CombatLogDelegate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/style/ColorRepository.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/style/StyleRepository.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/custom_widgets/IconSelector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/helper/TomlHelper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/view/InitiativeView.cpp
)

set(UI_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/cyrusdialog/CyrusDialog.ui
)

set(RESOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/asset/icon.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/asset/image.qrcs
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
    Qt6::Core
    Qt6::Widgets 
    Qt6::Gui
    Qt6::Sql
    tomlplusplus::tomlplusplus
)

target_include_directories(
    qtLib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/cyrusdialog
    ${CMAKE_CURRENT_SOURCE_DIR}/asset
    ${CMAKE_CURRENT_SOURCE_DIR}/custom_widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/view
    ${CMAKE_CURRENT_SOURCE_DIR}/asset/icon
    ${CMAKE_CURRENT_SOURCE_DIR}/asset/image
)