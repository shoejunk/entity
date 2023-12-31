cmake_minimum_required(VERSION 3.14...3.22)

project(entity_standalone LANGUAGES CXX)

# Disable warning: _M_FP_PRECISE is defined in current command line and not in module command line.
# This warning is caused by importing std.core and I can't for the life of me figure out a way to fix it.
# Also somehow just wrapping the import in a #pragma warning(disable: 5050) doesn't work.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd5050")

# --- Import tools ----
include(../cmake/tools.cmake)

# ---- Dependencies ----
include(../cmake/CPM.cmake)

CPMAddPackage(
    GITHUB_REPOSITORY jarro2783/cxxopts
    VERSION 3.0.0
    OPTIONS "CXXOPTS_BUILD_EXAMPLES NO" "CXXOPTS_BUILD_TESTS NO" "CXXOPTS_ENABLE_INSTALL YES"
)

CPMAddPackage(NAME entity SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..)

# Add package: shoejunk/log
CPMAddPackage(
	NAME log
	GITHUB_REPOSITORY shoejunk/log
	VERSION 1.0.0
)

# ---- Create standalone executable ----
file(GLOB sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)

add_executable(${PROJECT_NAME} ${sources})

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 23 OUTPUT_NAME "entity")

target_link_libraries(${PROJECT_NAME} entity log cxxopts)
