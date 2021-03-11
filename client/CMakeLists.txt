cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(PIClient LANGUAGES CXX)


set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS_DEBUG "-g")
# --- Import tools ----

include(../cmake/tools.cmake)

# ---- Dependencies ----

include(../cmake/CPM.cmake)

CPMAddPackage(
  GITHUB_REPOSITORY jarro2783/cxxopts
  VERSION 2.2.0
  OPTIONS "CXXOPTS_BUILD_TESTS NO"
)

CPMAddPackage(NAME Prinsight SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..)

# ---- Create standalone executable ----

file(GLOB sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)

add_executable(piclient ${sources})

target_include_directories(
  piclient
  PRIVATE  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../source/>
)

set_target_properties(piclient PROPERTIES CXX_STANDARD 17 OUTPUT_NAME "piclient")

target_link_libraries(piclient Prinsight::prinsight cxxopts)
