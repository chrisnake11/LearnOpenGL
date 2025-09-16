#version 330 core

in vec3 Position;
in vec3 Normal;

out vec4 FragColor;

uniform samplerCube texture1;
uniform vec3 cameraPos;


void main(){
    float ratio = 1.00 / 1.52;
    vec3 I = normalize(Position - cameraPos);
    vec3 R = refract(I, normalize(Normal), ratio);
//    vec3 R = reflect(I, normalize(Normal));

    FragColor = vec4(texture(texture1, R).rgb, 1.0);
}