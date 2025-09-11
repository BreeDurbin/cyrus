add_library(
    controller 
    STATIC
    CharacterController.h
    CharacterController.cpp
    CastState.h
    TomlHelper.h
    TomlHelper.cpp
)

target_include_directories(controller PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(
    controller
    PUBLIC
    shared
    model
    delegate
    view
    tomlplusplus::tomlplusplus
)