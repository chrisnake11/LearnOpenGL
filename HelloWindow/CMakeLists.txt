cmake_minimum_required(VERSION 3.14)
project(ComputerGraphicsDemo)

set(CMAKE_CXX_STANDARD 17)

set(GLFW_DIR "D:/dev/ComputerGraphics/glfw-3.4")

include_directories("${GLFW_DIR}/include")
link_directories("${GLFW_DIR}/lib")

add_executable(ComputerGraphicsDemo demo.cpp glad.c)
target_include_directories(ComputerGraphicsDemo PRIVATE "${GLFW_DIR}/include")

target_compile_definitions(ComputerGraphicsDemo PRIVATE GLFW_STATIC)

target_link_libraries(ComputerGraphicsDemo PRIVATE glfw3)

if(WIN32)
    target_link_libraries(ComputerGraphicsDemo PRIVATE opengl32)
else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(ComputerGraphicsDemo PRIVATE OpenGL::GL)
endif()


