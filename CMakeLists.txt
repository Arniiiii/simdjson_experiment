cmake_minimum_required(VERSION 3.14...3.22)

# ---- Project ----

# Note: update this to your new project's name and version
project(
  simdjson_experiment
  VERSION 1.0
  LANGUAGES CXX)

option(BUILD_SHARED_LIBS "yes/no" NO)

if(${BUILD_SHARED_LIBS})
  # https://stackoverflow.com/questions/33062728/cmake-link-shared-library-on-windows
  if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
  endif()
endif()

set(FETCHCONTENT_QUIET
    OFF
    CACHE BOOL "Make downloading of packages quiet")

# ---- Include guards ----

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

# ---- Create library ----

# Note: for header-only libraries change all PUBLIC flags to INTERFACE and
# create an interface target: add_library(${PROJECT_NAME} INTERFACE)
add_executable(${PROJECT_NAME} main.cpp)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)

# being a cross-platform target, we enforce standards conformance on MSVC
target_compile_options(${PROJECT_NAME}
                       PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive->")

# ---- Add dependencies via CPM ----
# see https://github.com/TheLartians/CPM.cmake for more info

include(getCPM.cmake)

set(packageProject_VERSION 1.12.0)
cpmaddpackage(
  NAME
  PackageProject.cmake
  VERSION
  ${packageProject_VERSION}
  URL
  "https://github.com/TheLartians/PackageProject.cmake/archive/refs/tags/v${packageProject_VERSION}.zip"
)

set(simdjson_VERSION 3.10.1)
# cpmaddpackage(
#   NAME
#   simdjson
#   VERSION
#   "${simdjson_VERSION}"
#   URL
#   "https://github.com/simdjson/simdjson/archive/refs/tags/v${simdjson_VERSION}.tar.gz"
#   OPTIONS
#   "SIMDJSON_ENABLE_THREADS ON"
#   "BUILD_SHARED_LIBS ON"
#   "SIMDJSON_BUILD_STATIC_LIB OFF" 
# )
file(DOWNLOAD
  https://github.com/simdjson/simdjson/releases/download/v${simdjson_VERSION}/simdjson.cpp
./simdjson/simdjson.cpp)
file(DOWNLOAD
  https://github.com/simdjson/simdjson/releases/download/v${simdjson_VERSION}/simdjson.h
  ./simdjson/include/simdjson/simdjson.h)

add_library(simdjson ./simdjson/simdjson.cpp)
target_include_directories(
  ${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/simdjson/include>
                         $<INSTALL_INTERFACE:include/simdjson/${PROJECT_NAME}>
)

# the location where the project's version header will be placed should match the project's regular
# header paths
string(TOLOWER simdjson/version.h simdjson_VERSION_HEADER_LOCATION)
string(TOLOWER simdjson/export.h simdjson_EXPORT_HEADER_LOCATION)

set_property(TARGET simdjson PROPERTY VERSION ${simdjson_VERSION})
set_property(TARGET simdjson PROPERTY SOVERSION ${simdjson_VERSION})

packageProject(
  NAME simdjson
  VERSION ${simdjson_VERSION}
  NAMESPACE ${simdjson}
  BINARY_DIR ${PROJECT_BINARY_DIR}
  INCLUDE_DIR ${PROJECT_SOURCE_DIR}/simdjson/include
  INCLUDE_DESTINATION include/simdjson
  VERSION_HEADER "${simdjson_VERSION_HEADER_LOCATION}" EXPORT_HEADER
  "${simdjson_EXPORT_HEADER_LOCATION}"
  COMPATIBILITY "AnyNewerVersion" DISABLE_VERSION_SUFFIX ON
)


target_link_libraries(${PROJECT_NAME} PUBLIC simdjson)

set_property(TARGET ${PROJECT_NAME} PROPERTY VERSION ${PROJECT_VERSION})
set_property(TARGET ${PROJECT_NAME} PROPERTY SOVERSION 1)

packageproject(
  NAME
  ${PROJECT_NAME}
  VERSION
  ${PROJECT_VERSION}
  BINARY_DIR
  ${PROJECT_BINARY_DIR}
  COMPATIBILITY
  "AnyNewerVersion"
  DISABLE_VERSION_SUFFIX
  ON
  DEPENDENCIES
  "simdjson ${simdjson_VERSION}"
)
