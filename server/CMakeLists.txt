cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(PIServer LANGUAGES CXX)


set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS_DEBUG "-g")
# --- Import tools ----

include(../cmake/tools.cmake)

# ---- Dependencies ----

include(../cmake/CPM.cmake)

#CPMAddPackage("gh:pistacheio/pistache#master")
# pistache
CPMAddPackage(
  NAME pistache
  GIT_TAG master
  GITHUB_REPOSITORY pistacheio/pistache
  OPTIONS "BUILD_SHARED_LIBS YES" # build sharedibs
  OPTIONS "PISTACHE_INSTALL YES" # create an installable target
  OPTIONS "PISTACHE_PIC YES"
)

# ---- Create standalone executable ----

file(GLOB sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)

add_executable(piserver ${sources})

target_include_directories(
  piserver
  PRIVATE  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../source/>
)

set_target_properties(piserver PROPERTIES CXX_STANDARD 17 OUTPUT_NAME "piserver")

target_link_libraries(piserver pthread pistache)
