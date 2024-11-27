cmake_minimum_required(VERSION 3.10)
project(ShellProject)

# Set default build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type (default Debug)" FORCE)
endif()

# Set CMake to output files in the build directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Enable testing
enable_testing()

# Add include directories
include_directories(include)

# Shell executable
add_executable(shell
    src/shell.c
    src/monitor_cmds.c
    src/execute_cmds.c
    src/shell_built_in_cmds.c
)

# Copy the entire tests folder to the build directory
file(COPY ${CMAKE_SOURCE_DIR}/tests DESTINATION ${CMAKE_BINARY_DIR})

# Test executable with Unity dependency from Conan
add_executable(test_shell
    tests/test_shell_built_in_cmds.c
    src/shell_built_in_cmds.c
)

# Set the output directory for test executables to be within build/tests
set_target_properties(test_shell PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)

# Find Unity dependency from Conan
find_package(unity REQUIRED)

# Link Unity library and gcov to the test executable
target_link_libraries(test_shell unity::unity gcov)

# Enable code coverage flags for tests
target_compile_options(test_shell PRIVATE -fprofile-arcs -ftest-coverage)

# Register the test executable with CTest
add_test(NAME test_shell COMMAND ${CMAKE_BINARY_DIR}/tests/test_shell)
