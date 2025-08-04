cmake_minimum_required(VERSION 3.16)
project(UILibrary)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set static MSVC runtime for Debug and Release
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE STRING "" FORCE)
    add_definitions(-DNOMINMAX)
endif()

# Fetch SFML using FetchContent
include(FetchContent)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        2.6.1
)

# Configure SFML to build statically and fetch its own dependencies
set(SFML_USE_SYSTEM_DEPS OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_TESTING OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(SFML)

# Source files
set(SOURCES
    src/ElementManager.cpp
    src/assetManager.cpp
)

# Build UILibrary as a static library
add_library(UILibrary STATIC ${SOURCES})

# Include paths for UILibrary
target_include_directories(UILibrary PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
PRIVATE
    src
)

# Define SFML_STATIC for correct linkage
target_compile_definitions(UILibrary PUBLIC SFML_STATIC)

# Link required SFML libraries
target_link_libraries(UILibrary PUBLIC sfml-graphics sfml-window sfml-system sfml-audio)

# Install rules
include(GNUInstallDirs)
install(TARGETS UILibrary
    EXPORT UILibraryTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT UILibraryTargets
    FILE UILibraryTargets.cmake
    NAMESPACE UILibrary::
    DESTINATION lib/cmake/UILibrary
)

# Option to build examples
option(BUILD_EXAMPLES "Build the examples" ON)

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
