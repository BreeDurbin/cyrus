add_library(model STATIC
    data/AttackRate.h
    data/AttackAction.cpp
    data/AttackAction.h
    data/Character.cpp
    data/Character.h
    data/CastAction.cpp
    data/CastAction.h
    data/MiscAction.cpp
    data/MiscAction.h
    CharacterModel.cpp
    CharacterModel.h
)

target_include_directories(
    model 
    PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}    
    ${CMAKE_CURRENT_SOURCE_DIR}/data
)

target_link_libraries(
    model
    PUBLIC
    shared
    style
)