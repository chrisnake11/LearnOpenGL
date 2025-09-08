#version 460 core

in vec2 TexCoord;
out vec4 frag_color;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    vec4 texel0, texel1, result_color;

    texel0 = texture(texture1, TexCoord);
    texel1 = texture(texture2, TexCoord);

    result_color = mix(texel0, texel1, texel1.a * 0.2);

    frag_color = result_color;
}