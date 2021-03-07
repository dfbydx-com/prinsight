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

CPMAddPackage(
  NAME fmt
  GIT_TAG 7.1.3
  GITHUB_REPOSITORY fmtlib/fmt
  OPTIONS "FMT_INSTALL YES" # create an installable target
)

# ---- Add source files ----

# Note: globbing sources is considered bad practice as CMake's generators may not detect new files
# automatically. Keep that in mind when changing files, or explicitly mention them here.
file(GLOB_RECURSE headers CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

# ---- Create library ----
# set_property(TARGET ${target} PROPERTY C_STANDARD 99)
# set(CMAKE_C_FLAGS "-std=c99")
add_definitions(-DC99)

add_library(prinsight SHARED ${headers} ${sources})

set_target_properties(prinsight PROPERTIES CXX_STANDARD 17)

# being a cross-platform target, we enforce standards conformance on MSVC
target_compile_options(prinsight PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive>")

# Link dependencies
target_link_libraries(prinsight PUBLIC fmt::fmt)

target_include_directories(
  prinsight PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                 $<INSTALL_INTERFACE:include/${PROJECT_NAME}-${PROJECT_VERSION}>
)

# 3d party dependency libraries of CiFEr
set (DEPS_DIR ${PROJECT_SOURCE_DIR}/out/prebuilt/linux) # FIXME
set(DEPS_LIBRARIES cifer gmpxx gmp spdlog)

target_link_libraries(
  prinsight PRIVATE
    ${DEPS_DIR}/cifer/include
    ${DEPS_DIR}/gmp/include
    ${DEPS_DIR}/spdlog/include
    )

target_include_directories(
  prinsight PRIVATE
    ${DEPS_DIR}/cifer/lib
    ${DEPS_DIR}/gmp/lib
    ${DEPS_DIR}/spdlog/lib
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
  DEPENDENCIES "fmt 7.1.3"
)
