cmake_minimum_required(VERSION 3.10)
project(monitoring_programm)

# Include header directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/lib)

# Copy config.json to the build directory
# configure_file(${CMAKE_SOURCE_DIR}/config.json ${CMAKE_BINARY_DIR}/config.json COPYONLY)

# Add the executable
add_executable(monitoring_programm
    src/main.c
    src/metrics.c
    src/expose_metrics.c
    src/cJSON.c  #Added cJSON source file
)

# Add the directory where shared libraries are installed (for libraries installed with sudo make install)
link_directories(/usr/local/lib)

# Link libraries for libprom, libpromhttp, and pthread
target_link_libraries(monitoring_programm
    /usr/local/lib/libprom.so
    /usr/local/lib/libpromhttp.so
    pthread
)
