add_library(model STATIC
    AttackAction.cpp
    AttackAction.h
    Character.cpp
    Character.h
    CastAction.cpp
    CastAction.h
    CharacterModel.cpp
    CharacterModel.h
)

target_include_directories(
    model 
    PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
    model
    PUBLIC
    shared
    style
)