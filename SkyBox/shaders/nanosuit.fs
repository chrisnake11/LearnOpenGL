#version 460 core

in vec2 TexCoords;
out vec4 FragColor;

// 统一用数组表示多个贴图
uniform sampler2D texture_diffuse[8];
uniform sampler2D texture_specular[8];
uniform sampler2D texture_normal[8];
uniform sampler2D texture_height[8];
uniform sampler2D texture_reflection[8];

// 由 CPU 传入每种贴图的实际数量
uniform int numDiffuse;
uniform int numSpecular;
uniform int numNormal;

void main()
{
    vec4 diffuseColor = vec4(0.0);
    vec4 specularColor = vec4(0.0);
    vec4 normalColor = vec4(0.0);

    // 遍历 diffuse
    for (int i = 0; i < numDiffuse; i++) {
        diffuseColor += texture(texture_diffuse[i], TexCoords);
    }

    // 遍历 specular
    for (int i = 0; i < numSpecular; i++) {
        specularColor += texture(texture_specular[i], TexCoords);
    }

    // 如果有法线贴图
    if (numNormal > 0) {
        normalColor = texture(texture_normal[0], TexCoords);
    }

    // 最简单的合成方式（你可以改成 PBR 或 Blinn-Phong）
    FragColor = diffuseColor + 0.3 * specularColor;
}
