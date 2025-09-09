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

# 拷贝着色器文件和png文件到可执行文件目录
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/vs.glsl DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/fs.glsl DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/awesomeface.png DESTINATION ${CMAKE_CURRENT_BINARY_DIR})



