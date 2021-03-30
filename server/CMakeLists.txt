cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(PIServer LANGUAGES CXX)


set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS_DEBUG "-g")
# --- Import tools ----

include(../cmake/tools.cmake)

# ---- Dependencies ----

include(../cmake/CPM.cmake)

# logging
CPMAddPackage(
  NAME spdlog
  GIT_TAG v1.8.2
  GITHUB_REPOSITORY gabime/spdlog
  OPTIONS "SPDLOG_INSTALL YES" # create an installable target
  OPTIONS "CMAKE_POSITION_INDEPENDENT_CODE ON"
)

# pistache
CPMAddPackage(
  NAME pistache
  GIT_TAG master
  GITHUB_REPOSITORY pistacheio/pistache
  OPTIONS "BUILD_SHARED_LIBS YES" # build sharedibs
  OPTIONS "PISTACHE_INSTALL YES" # create an installable target
  OPTIONS "PISTACHE_PIC YES"
)

# json support
SET(CMAKE_CXX_FLAGS "-Wall -std=c++11")
CPMAddPackage(
  NAME nlohmann_json
  VERSION 3.9.1
  # not using the repo as it takes forever to clone
  URL https://github.com/nlohmann/json/releases/download/v3.9.1/include.zip
  URL_HASH SHA256=6bea5877b1541d353bd77bdfbdb2696333ae5ed8f9e8cc22df657192218cad91
)

if(nlohmann_json_ADDED)
  add_library(nlohmann_json INTERFACE)
  target_include_directories(nlohmann_json SYSTEM INTERFACE ${nlohmann_json_SOURCE_DIR}/include)
endif()

CPMAddPackage(NAME Prinsight SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..)

# ---- Create standalone executable ----

file(GLOB sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)

add_executable(piserver ${sources})

target_include_directories(
  piserver
  PRIVATE  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../source/>
)

set_target_properties(piserver PROPERTIES CXX_STANDARD 17 OUTPUT_NAME "piserver")

target_link_libraries(piserver Prinsight::prinsight spdlog::spdlog nlohmann_json pthread pistache)
