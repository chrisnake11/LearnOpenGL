#version 460 core
in vec3 color;
out vec4 fraged_color;
void main()
{
    fraged_color = vec4(color, 1.0);
}