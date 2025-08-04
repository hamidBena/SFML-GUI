cmake_minimum_required(VERSION 3.16)
project(UILibrary)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Enable LTO if supported
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

# Use static MSVC runtime
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE STRING "" FORCE)
    add_definitions(-DNOMINMAX)
endif()

# Force SFML to build statically
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build SFML as static libraries" FORCE)

# Extract SFML if needed
set(SFML_ZIP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SFML-2.6.1.rar")
set(SFML_EXTRACT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SFML-2.6.1")

if(NOT EXISTS "${SFML_EXTRACT_DIR}/CMakeLists.txt")
    message(STATUS "Extracting SFML from zip...")
    file(ARCHIVE_EXTRACT INPUT "${SFML_ZIP_PATH}" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty")
endif()

# Add SFML subdirectory
add_subdirectory(thirdparty/SFML-2.6.1)

# Collect all .cpp files in src/
file(GLOB_RECURSE UILIBRARY_SOURCES CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/src/*.cpp
)

# Create static library from sources
add_library(UILibrary STATIC ${UILIBRARY_SOURCES})

# Include headers
target_include_directories(UILibrary PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/thirdparty/SFML-2.6.1/include
)

# Define for SFML static linking
target_compile_definitions(UILibrary PUBLIC SFML_STATIC)

# Link SFML libs statically
target_link_libraries(UILibrary PUBLIC
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
)
