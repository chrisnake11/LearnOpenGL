#version 460 core
in vec3 aPos;
uniform vec2 offset; // xy轴移动偏移量
void main()
{
    gl_Position = vec4(aPos.xy + offset, aPos.z, 1.0);
}