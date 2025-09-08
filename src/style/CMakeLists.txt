add_library(style STATIC
    ColorRepository.cpp
    ColorRepository.h
    StyleRepository.cpp
    StyleRepository.h
    IconRepository.h
)

target_include_directories(style PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(style
    PUBLIC
        shared
)