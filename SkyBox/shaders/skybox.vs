#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aPos; // 顶点坐标作为skybox的纹理采样坐标
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww; // 设置w分量为1.0以确保深度值为1.0
}
