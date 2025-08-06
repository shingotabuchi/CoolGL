cmake_minimum_required(VERSION 3.15)
project(CoolGL LANGUAGES C CXX)

# ———————————————————————
# 1) Standards & include path
# ———————————————————————
set(CMAKE_C_STANDARD    11)
set(CMAKE_CXX_STANDARD  17)
set(CMAKE_C_STANDARD_REQUIRED    ON)
set(CMAKE_CXX_STANDARD_REQUIRED  ON)
include_directories(${PROJECT_SOURCE_DIR}/include)

# ———————————————————————
# 2) Build static libraries
# ———————————————————————
add_library(glad STATIC src/glad.c)
target_include_directories(glad PUBLIC ${PROJECT_SOURCE_DIR}/include)
file(GLOB_RECURSE SOIL2_SRC
    "${CMAKE_SOURCE_DIR}/third_party/SOIL2/src/*.c"
)
add_library(SOIL2 STATIC ${SOIL2_SRC})
target_include_directories(SOIL2 PUBLIC
    "${PROJECT_SOURCE_DIR}/third_party/SOIL2/src"
)
target_compile_definitions(SOIL2  PUBLIC GL_SILENCE_DEPRECATION)

# ———————————————————————
# 3) Find external libs
# ———————————————————————
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

# platform-specific frameworks
if(APPLE)
  find_library(COCOA_FRAMEWORK     Cocoa)
  find_library(IOKIT_FRAMEWORK     IOKit)
  find_library(COREVIDEO_FRAMEWORK CoreVideo)
  find_library(OPENGL_FRAMEWORK    OpenGL)
  set(PLAT_LIBS
    ${COCOA_FRAMEWORK}
    ${IOKIT_FRAMEWORK}
    ${COREVIDEO_FRAMEWORK}
    ${OPENGL_FRAMEWORK}
  )
elseif(WIN32)
  set(PLAT_LIBS opengl32)
endif()

# everything both demos link against
set(COMMON_LIBS
  glad
  glfw
  glm::glm
  ${PLAT_LIBS}
  SOIL2
)

# ———————————————————————
# 4) helper to add a demo
# ———————————————————————
function(add_demo target_name source_file)
  add_executable(${target_name} ${source_file})
  target_link_libraries(${target_name} PRIVATE ${COMMON_LIBS})
endfunction()

# ———————————————————————
# 5) list your demos here
# ———————————————————————
add_demo(dancing_triangle src/dancing_triangle.cpp)
add_demo(doge src/doge.cpp)
add_demo(experiment src/experiment.cpp)
