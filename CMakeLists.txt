cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

# ---- Project ----

# Note: update this to your new project's name and version
project(
  Prinsight
  VERSION 0.1.0
  LANGUAGES C CXX
)

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
#CPMAddPackage(
#  NAME spdlog
#  GIT_TAG v1.8.2
#  GITHUB_REPOSITORY gabime/spdlog
#  OPTIONS "CMAKE_POSITION_INDEPENDENT_CODE ON"
#)

# CiFEr and it's dependency libraries
set (CIFER_DEPS_DIR ${PROJECT_SOURCE_DIR}/out/prebuilt/linux) # FIXME
set(CIFER_DEPS_LIBRARIES cifer gmpxx gmp)

set(CIFER_DEPS_LIB_DIRS
    ${CIFER_DEPS_DIR}/cifer/lib
    ${CIFER_DEPS_DIR}/gmp/lib
    )

set(CIFER_DEPS_INCLUDE_DIRS
    ${CIFER_DEPS_DIR}/cifer/include
    ${CIFER_DEPS_DIR}/gmp/include
    )
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

set_target_properties(prinsight PROPERTIES CXX_STANDARD 17)

# being a cross-platform target, we enforce standards conformance on MSVC
target_compile_options(prinsight PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive>")

target_link_libraries(
  prinsight
  PUBLIC ${CIFER_DEPS_LIBRARIES}
)

target_include_directories(
  prinsight
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                 $<INSTALL_INTERFACE:include/${PROJECT_NAME}-${PROJECT_VERSION}>
  ${CIFER_DEPS_LIB_DIRS}
)

target_link_directories(prinsight PUBLIC ${CIFER_DEPS_LIB_DIRS})

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
  DEPENDENCIES ""
)
