add_library(controller STATIC
    InitiativeItemController.h
    InitiativeController.h
)

target_include_directories(controller PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(controller
    PUBLIC
        shared
        model
        delegate
)