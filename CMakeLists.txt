cmake_minimum_required(VERSION 3.18)
project(gl_texture_tutorial LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

set(OpenGL_GL_PREFERENCE "GLVND")

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.10
)

FetchContent_Declare(
  glew
  URL https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.zip
  SOURCE_SUBDIR build/cmake
  DOWNLOAD_EXTRACT_TIMESTAMP ON
)

FetchContent_Declare(
  stb
  GIT_REPOSITORY https://github.com/nothings/stb.git
  GIT_TAG master
)

set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "")
set(GLFW_INSTALL OFF CACHE INTERNAL "")
set(BUILD_UTILS OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(glfw glew stb)

add_executable(gl_texture_tutorial main.cpp)

# Add stb directory as include path
target_include_directories(gl_texture_tutorial PRIVATE
    ${glew_SOURCE_DIR}/include
    ${stb_SOURCE_DIR}
)

target_link_libraries(gl_texture_tutorial
    glfw
    glew_s
)

set(TEXTURE_SOURCE "${CMAKE_SOURCE_DIR}/crate.png")
set(TEXTURE_DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/crate.png")

add_custom_command(
    OUTPUT ${TEXTURE_DESTINATION}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${TEXTURE_SOURCE} ${TEXTURE_DESTINATION}
    DEPENDS ${TEXTURE_SOURCE}
    COMMENT "Copying crate.png to build directory"
)

add_custom_target(copy_textures ALL DEPENDS ${TEXTURE_DESTINATION})

add_dependencies(gl_texture_tutorial copy_textures)
