cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(PIClient LANGUAGES CXX)


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
)

CPMAddPackage(
  GITHUB_REPOSITORY jarro2783/cxxopts
  VERSION 2.2.0
  OPTIONS "CXXOPTS_BUILD_TESTS NO"
)

# curl
find_package(CURL REQUIRED)
#CPMAddPackage(gh:curl/curl@curl-7_75_0)
#CPMAddPackage(
#  NAME curl
#  GIT_TAG curl-7_75_0
#  GITHUB_REPOSITORY curl/curl
#)

# Rest http client
CPMAddPackage(
  NAME RESTinCurl
  GIT_TAG master
  GITHUB_REPOSITORY jgaa/RESTinCurl
  DOWNLOAD_ONLY YES
)

if(RESTinCurl_ADDED)
  add_library(RESTinCurl INTERFACE)
  target_include_directories(RESTinCurl SYSTEM INTERFACE ${RESTinCurl_SOURCE_DIR}/include)
endif()


CPMAddPackage(NAME Prinsight SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..)

# ---- Create standalone executable ----

file(GLOB sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)

add_executable(piclient ${sources})

target_include_directories(
  piclient
  PRIVATE  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../source/>
)

set_target_properties(piclient PROPERTIES CXX_STANDARD 17 OUTPUT_NAME "piclient")

target_link_libraries(piclient Prinsight::prinsight ${CURL_LIBRARIES}  RESTinCurl spdlog::spdlog cxxopts)
