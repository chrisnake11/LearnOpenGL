cmake_minimum_required(VERSION 3.14)
project(main)

set(CMAKE_CXX_STANDARD 17)

set(GLFW_DIR "D:/dev/ComputerGraphics/glfw-3.4")
set(GLM_DIR "D:/dev/ComputerGraphics/glm")
set(ASSIMP_DIR "D:/dev/ComputerGraphics/assimp")

include_directories("${GLFW_DIR}/include" "${GLM_DIR}" "${ASSIMP_DIR}/include")
link_directories("${GLFW_DIR}/lib" "${ASSIMP_DIR}/lib")

add_executable(main main.cpp
        Shader.cpp
        Camera.cpp
        glad.c
)

target_include_directories(main PRIVATE "${GLFW_DIR}/include" "${GLM_DIR}" "${ASSIMP_DIR}/include")

target_compile_definitions(main PRIVATE GLFW_STATIC)

target_link_libraries(main PRIVATE glfw3 assimp-vc143-mtd)

if(WIN32)
    target_link_libraries(main PRIVATE opengl32)
else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(main PRIVATE OpenGL::GL)
endif()

# 收集着色器与纹理(根据你的实际目录调整)
file(GLOB SHADER_FILES
        "${CMAKE_SOURCE_DIR}/shaders/*"
)

# 构建后复制到可执行文件所在配置目录 (Debug / Release)
add_custom_command(TARGET main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Copy assets to: $<TARGET_FILE_DIR:main>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SHADER_FILES} $<TARGET_FILE_DIR:main>
        VERBATIM
)
