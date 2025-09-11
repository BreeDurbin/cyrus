# Generate resource sources
qt_add_resources(asset_resources
    icon.qrc
    image.qrc
)

# Wrap them in an object library so "asset" is a real target
add_library(asset OBJECT ${asset_resources})