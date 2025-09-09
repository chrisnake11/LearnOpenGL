#version 460 core

out vec4 frag_color;

uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
    frag_color = vec4(objectColor * lightColor, 1.0);
}