include(FetchContent)

# ---- toml++ ----
# Vendored in third_party so the build works offline.
FetchContent_Declare(
  tomlplusplus
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/third_party/tomlplusplus-3.4.0
)
FetchContent_MakeAvailable(tomlplusplus)


# ---- Qt6 ----
# Qt's package dirs are cached, so switching modes in an existing build dir
# would silently keep the old Qt.
if(DEFINED CACHE{_CYRUS_STANDALONE_CONFIGURED} AND
   NOT _CYRUS_STANDALONE_CONFIGURED STREQUAL CYRUS_STANDALONE)
  message(FATAL_ERROR
    "CYRUS_STANDALONE changed from ${_CYRUS_STANDALONE_CONFIGURED} to ${CYRUS_STANDALONE} "
    "in an existing build directory. Use a separate build directory for each mode.")
endif()
set(_CYRUS_STANDALONE_CONFIGURED ${CYRUS_STANDALONE} CACHE INTERNAL "")

if(CYRUS_STANDALONE)
  # Headless Qt vendored in third_party/qt6: offscreen/minimal/vnc platforms only,
  # no system deps beyond libc/libstdc++.
  set(_cyrus_qt_prefix ${CMAKE_SOURCE_DIR}/third_party/qt6)
  # Qt resolves its own tool packages (Qt6GuiTools, ...) via CMAKE_PREFIX_PATH,
  # so the vendored prefix must come first there too.
  list(PREPEND CMAKE_PREFIX_PATH ${_cyrus_qt_prefix})
  find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui Sql
    PATHS ${_cyrus_qt_prefix}
    NO_DEFAULT_PATH)
else()
  find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui Sql)
endif()
message(STATUS "Using Qt ${Qt6_VERSION} from ${Qt6_DIR}")
