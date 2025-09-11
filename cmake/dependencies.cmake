include(FetchContent)

# ---- toml++ ----
FetchContent_Declare(
  tomlplusplus
  GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
  GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(tomlplusplus)


# ---- Qt6 ----
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui Sql)
