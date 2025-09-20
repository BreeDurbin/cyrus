
# If you have other qrc files
qt_add_resources(asset_resources
    "${CMAKE_CURRENT_SOURCE_DIR}/control.qrc"
    "${CMAKE_CURRENT_SOURCE_DIR}/icon.qrc"
    "${CMAKE_CURRENT_SOURCE_DIR}/image.qrc"
    "${CMAKE_CURRENT_SOURCE_DIR}/font.qrc"
)

# Object library
add_library(asset OBJECT
    ${asset_resources}
)
