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

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build libraries as STATIC" FORCE)

add_subdirectory(third_party/SOIL2)
target_compile_definitions(soil2 PUBLIC GL_SILENCE_DEPRECATION)

set(ASSIMP_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES  OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT      ON  CACHE BOOL "" FORCE)
add_subdirectory(third_party/assimp)

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
  soil2
  assimp
)

# ———————————————————————
# 4) engine library
# ———————————————————————
file(GLOB ENGINE_SOURCES CONFIGURE_DEPENDS
  ${PROJECT_SOURCE_DIR}/src/engine/*.cpp
)
add_library(engine STATIC ${ENGINE_SOURCES})
target_include_directories(engine PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(engine PUBLIC ${COMMON_LIBS})

# ———————————————————————
# 5) helper to add a demo
# ———————————————————————
function(add_demo target_name source_file)
  add_executable(${target_name} ${source_file})
  target_link_libraries(${target_name} PRIVATE engine ${COMMON_LIBS})

  # --- FIX ADDED HERE ---
  # Set execute permissions for the target on Unix-like systems (macOS, Linux)
  # This prevents "Permission denied" errors when running from the Makefile.
  if(UNIX)
    set_target_properties(${target_name} PROPERTIES
      PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE
    )
  endif()
endfunction()

# ———————————————————————
# 6) automatically discover and build demos
# ———————————————————————
file(GLOB DEMO_SOURCES "${PROJECT_SOURCE_DIR}/src/demo/*.cpp")
foreach(DEMO_SOURCE ${DEMO_SOURCES})
  # Extract filename without extension to use as target name
  get_filename_component(DEMO_NAME ${DEMO_SOURCE} NAME_WE)
  add_demo(${DEMO_NAME} ${DEMO_SOURCE})
  message(STATUS "Added demo: ${DEMO_NAME}")
endforeach()
