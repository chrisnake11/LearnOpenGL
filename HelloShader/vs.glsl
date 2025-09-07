#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

out vec3 ourColor; // 传递给片段着色器的颜色

void main()
{
    gl_Position = vec4(aPos, 1.0);
    ourColor = aColor;
}