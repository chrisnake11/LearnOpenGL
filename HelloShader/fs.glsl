#version 460 core

uniform float greenValue;
out vec4 frag_color;
void main()
{
    frag_color = vec4(0.0, greenValue, 0.0, 1.0);
}