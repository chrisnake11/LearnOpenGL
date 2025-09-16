#version 330 core

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_reflection1; // reflection intensity mask
uniform samplerCube skybox; // sampler from skybox

uniform vec3 viewPos;

out vec4 FragColor;

void main()
{
    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos - FragPos);
    vec3 R = reflect(-V, N);

    vec3 diff_texture = texture(texture_diffuse1, TexCoords).rgb;
    float reflMask = texture(texture_reflection1, TexCoords).r;

    // diff
    vec3 diff = diff_texture;

    // refl
    vec3 refl = texture(skybox, R).rgb; // sampler from skybox texture

    vec3 result = mix(diff, refl, reflMask);

    FragColor = vec4(result, 1.0);
}

















