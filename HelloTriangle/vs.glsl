#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
uniform vec2 offset; // xy轴移动偏移量
out vec3 color; // 传递给片段着色器的颜色
void main()
{
    gl_Position = vec4(aPos.xy + offset, aPos.z, 1.0);
    color = aColor;
}