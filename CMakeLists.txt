cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

# ---- Project ----

# Note: update this to your new project's name and version
project(
  Prinsight
  VERSION 0.1.0
  LANGUAGES C CXX
)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS_DEBUG "-g")

# ---- Include guards ----

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

# ---- Add dependencies via CPM ----
# see https://github.com/TheLartians/CPM.cmake for more info

include(cmake/CPM.cmake)

# PackageProject.cmake will be used to make our target installable
CPMAddPackage("gh:TheLartians/PackageProject.cmake@1.4.1")

# logging
CPMAddPackage(
  NAME spdlog
  GIT_TAG v1.8.2
  GITHUB_REPOSITORY gabime/spdlog
  OPTIONS "SPDLOG_INSTALL YES" # create an installable target
  OPTIONS "CMAKE_POSITION_INDEPENDENT_CODE ON"
)

# base64
CPMAddPackage(
  NAME base64
  GITHUB_REPOSITORY zhicheng/base64
  GIT_TAG master
  DOWNLOAD_ONLY YES
)

if (base64_ADDED)
  # base64 has no CMake support, so we create our own target

  FILE(GLOB base64_sources ${base64_SOURCE_DIR}/*.c)
  list(REMOVE_ITEM base64_sources "${base64_SOURCE_DIR}/main.c")
  add_library(base64 STATIC ${base64_sources})

  target_include_directories(base64
    PUBLIC
      $<BUILD_INTERFACE:${base64_SOURCE_DIR}>
  )
endif()

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

# CiFEr and it's dependency libraries
# FIXME use find_package and similar interfaces to use dependencies
# FIXME pass prebuilt from command line
set(CIFER_DEPS_DIR ${PROJECT_SOURCE_DIR}/out/prebuilt/linux)
find_library(CIFER_LIB cifer HINTS ${CIFER_DEPS_DIR}/cifer/lib REQUIRED NO_DEFAULT_PATH)
find_library(GMP_LIB gmp HINTS ${CIFER_DEPS_DIR}/gmp/lib REQUIRED NO_DEFAULT_PATH)
find_library(GMPXX_LIB gmpxx HINTS ${CIFER_DEPS_DIR}/gmp/lib REQUIRED NO_DEFAULT_PATH)
set(CIFER_DEPS_LIBRARIES ${GMP_LIB} ${GMPXX_LIB} ${CIFER_LIB})
message(STATUS "cifer dependency libs ${CIFER_DEPS_LIBRARIES}")

# set_property(TARGET ${target} PROPERTY C_STANDARD 99)
# set(CMAKE_C_FLAGS "-std=c99")
add_definitions(-DC99)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ---- Add source files ----
# Note: globbing sources is considered bad practice as CMake's generators may not detect new files
# automatically. Keep that in mind when changing files, or explicitly mention them here.
file(GLOB_RECURSE headers CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

# ---- Create library ----
add_library(prinsight SHARED ${headers} ${sources})

# set_target_properties(prinsight PROPERTIES CXX_STANDARD 17)

# being a cross-platform target, we enforce standards conformance on MSVC
target_compile_options(prinsight PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive>")

target_link_libraries(
  prinsight
  PRIVATE spdlog::spdlog base64 nlohmann_json
  PUBLIC ${CIFER_DEPS_LIBRARIES}
)

target_include_directories(
  prinsight
  PRIVATE $<BUILD_INTERFACE:${CIFER_DEPS_DIR}/cifer/include>
  PRIVATE $<BUILD_INTERFACE:${CIFER_DEPS_DIR}/gmp/include>
  PUBLIC  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
          $<INSTALL_INTERFACE:include/${PROJECT_NAME}-${PROJECT_VERSION}>
)

# ---- Create an installable target ----
# this allows users to install and find the library via `find_package()`.

# the location where the project's version header will be placed should match the project's regular
# header paths
string(TOLOWER ${PROJECT_NAME}/version.h VERSION_HEADER_LOCATION)

packageProject(
  NAME prinsight
  VERSION ${PROJECT_VERSION}
  NAMESPACE ${PROJECT_NAME}
  BINARY_DIR ${PROJECT_BINARY_DIR}
  INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include
  INCLUDE_DESTINATION include/${PROJECT_NAME}-${PROJECT_VERSION}
  VERSION_HEADER "${VERSION_HEADER_LOCATION}"
  COMPATIBILITY SameMajorVersion
  DEPENDENCIES " "
)
