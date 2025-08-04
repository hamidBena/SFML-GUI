cmake_minimum_required(VERSION 3.16)
project(UILibrary)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable interprocedural optimization (LTO)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

# Set static MSVC runtime for Debug and Release
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE STRING "" FORCE)
    add_definitions(-DNOMINMAX)
endif()

# Force SFML to build statically
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build SFML as static libraries" FORCE)

# Extract SFML zip archive if not already extracted
set(SFML_ZIP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SFML-2.6.1.rar")
set(SFML_EXTRACT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SFML-2.6.1")

if(NOT EXISTS "${SFML_EXTRACT_DIR}/CMakeLists.txt")
    message(STATUS "Extracting SFML from zip...")
    file(ARCHIVE_EXTRACT INPUT "${SFML_ZIP_PATH}" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty")
endif()


# Add SFML from thirdparty folder
add_subdirectory(thirdparty/SFML-2.6.1)

# Collect UI_Engine source files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS UI_Engine/src/*.cpp)

# Build UILibrary as static library
add_library(UILibrary STATIC ${SOURCES})

# Include paths for UILibrary
target_include_directories(UILibrary PUBLIC
    ${PROJECT_SOURCE_DIR}/UI_Engine/header
    ${PROJECT_SOURCE_DIR}/UI_Engine/widgets
    ${PROJECT_SOURCE_DIR}/thirdparty/SFML-2.6.1/include
)

# Define SFML_STATIC for correct linkage
target_compile_definitions(UILibrary PUBLIC SFML_STATIC)

# Link required SFML libraries
target_link_libraries(UILibrary
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
)

# Collect test sample files
file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "test samples/*.cpp")

# Build each test sample as separate executable
foreach(test_src IN LISTS TEST_SOURCES)
    get_filename_component(test_name ${test_src} NAME_WE)

    add_executable(${test_name} ${test_src})

    target_include_directories(${test_name} PRIVATE
        ${PROJECT_SOURCE_DIR}/UI_Engine/header
        ${PROJECT_SOURCE_DIR}/UI_Engine/widgets
        ${PROJECT_SOURCE_DIR}/thirdparty/SFML-2.6.1/include
    )

    target_compile_definitions(${test_name} PUBLIC SFML_STATIC)

    target_link_libraries(${test_name}
        UILibrary
        sfml-graphics
        sfml-window
        sfml-system
        sfml-audio
    )

endforeach()
