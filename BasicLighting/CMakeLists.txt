cmake_minimum_required(VERSION 3.14)
project(main)

set(CMAKE_CXX_STANDARD 17)

set(GLFW_DIR "D:/dev/ComputerGraphics/glfw-3.4")
set(GLM_DIR "D:/dev/ComputerGraphics/glm")

include_directories("${GLFW_DIR}/include" "${GLM_DIR}")
link_directories("${GLFW_DIR}/lib")

add_executable(main main.cpp Shader.cpp Camera.cpp glad.c)
target_include_directories(main PRIVATE "${GLFW_DIR}/include" "${GLM_DIR}")

target_compile_definitions(main PRIVATE GLFW_STATIC)

target_link_libraries(main PRIVATE glfw3)

if(WIN32)
    target_link_libraries(main PRIVATE opengl32)
else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(main PRIVATE OpenGL::GL)
endif()

# move *.fs and *.vs files to the build directory
file(GLOB SHADER_FILES "*.vs" "*.fs")
foreach(SHADER_FILE ${SHADER_FILES})
    configure_file(${SHADER_FILE} ${CMAKE_CURRENT_BINARY_DIR}/ COPYONLY)
endforeach()