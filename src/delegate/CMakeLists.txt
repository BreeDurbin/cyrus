add_library(delegate STATIC
    CombatLogDelegate.cpp
    CombatLogDelegate.h
    InitiativeDelegate.cpp
    InitiativeDelegate.h
    InitiativeLayout.cpp
    InitiativeLayout.h
    PainterUtils.cpp
    PainterUtils.h
)

target_include_directories(delegate PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(delegate PUBLIC shared model style)