cmake_minimum_required(VERSION 3.15)
project(CoolGL LANGUAGES C CXX)

# C/C++ standards
set(CMAKE_C_STANDARD    11)
set(CMAKE_CXX_STANDARD  17)
set(CMAKE_C_STANDARD_REQUIRED    ON)
set(CMAKE_CXX_STANDARD_REQUIRED  ON)

# Where to find our glad headers
include_directories(${PROJECT_SOURCE_DIR}/include)

# Build glad as a static C library
add_library(glad STATIC src/glad.c)
target_include_directories(glad PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Our executable from C++ source
add_executable(${PROJECT_NAME} src/main.cpp)

# Find GLFW and GLM (Homebrew on macOS, vcpkg or system on Windows)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# Link order: glad → windowing/math → platform frameworks
target_link_libraries(${PROJECT_NAME}
    glad
    glfw
    glm::glm
)

if(APPLE)
    find_library(COCOA_FRAMEWORK     Cocoa)
    find_library(IOKIT_FRAMEWORK     IOKit)
    find_library(COREVIDEO_FRAMEWORK CoreVideo)
    find_library(OPENGL_FRAMEWORK    OpenGL)
    target_link_libraries(${PROJECT_NAME}
        ${COCOA_FRAMEWORK}
        ${IOKIT_FRAMEWORK}
        ${COREVIDEO_FRAMEWORK}
        ${OPENGL_FRAMEWORK}
    )
elseif(WIN32)
    target_link_libraries(${PROJECT_NAME}
        opengl32
    )
endif()
