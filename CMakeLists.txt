cmake_minimum_required(VERSION 3.16)
project(UILibrary)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Force SFML to build as static
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build SFML as static libraries" FORCE)

# Add SFML from thirdparty folder
add_subdirectory(thirdparty/SFML-2.6.1)

# Enable link-time optimization
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

# Set MSVC runtime flags if using MSVC
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Collect UI_Engine source files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS UI_Engine/src/*.cpp)

# Build UILibrary as static
add_library(UILibrary STATIC ${SOURCES})

# Define headers
target_include_directories(UILibrary PUBLIC
    ${PROJECT_SOURCE_DIR}/UI_Engine/header
    ${PROJECT_SOURCE_DIR}/UI_Engine/widgets
    ${PROJECT_SOURCE_DIR}/thirdparty/SFML-2.6.1/include
)

# Define SFML_STATIC for proper linkage
target_compile_definitions(UILibrary PUBLIC SFML_STATIC)

# Link all required SFML components and dependencies
target_link_libraries(UILibrary
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
)

# Collect test sample source files
file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "test samples/*.cpp")

# Copy the assets directory to the build directory root
add_custom_target(copy_assets ALL
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${CMAKE_SOURCE_DIR}/assets"
	"${CMAKE_BINARY_DIR}/assets"
)


# Generate a test executable for each sample
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

	add_dependencies(${test_name} copy_assets)

endforeach()

