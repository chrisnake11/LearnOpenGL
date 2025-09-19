#version 460 core

in vec2 TexCoords;
out vec4 FragColor;

// 统一用数组表示多个贴图
uniform sampler2D texture_diffuse[10];
uniform sampler2D texture_specular[10];
uniform sampler2D texture_normal[10];
uniform sampler2D texture_height[10];
uniform sampler2D texture_reflection[10];

// 由 CPU 传入每种贴图的实际数量
uniform int numDiffuse;
uniform int numSpecular;
uniform int numNormal;
uniform int numHeight;
uniform int numReflection;

void main()
{
    vec3 diffuseColor = vec3(0.0);
    vec3 specularColor = vec3(0.0);
    vec3 normalColor = vec3(0.0);

    // 遍历 diffuse
    for (int i = 0; i < numDiffuse; i++) {
        diffuseColor += texture(texture_diffuse[i], TexCoords).rgb;
    }

    // 遍历 specular
    for (int i = 0; i < numSpecular; i++) {
        specularColor += texture(texture_specular[i], TexCoords).rgb;
    }

    // 如果有法线贴图
    if (numNormal > 0) {
        normalColor = texture(texture_normal[0], TexCoords).rgb;
    }

    // 最简单的合成方式（你可以改成 PBR 或 Blinn-Phong）
    FragColor = vec4((diffuseColor + 0.3 * specularColor), 1.0);
}
