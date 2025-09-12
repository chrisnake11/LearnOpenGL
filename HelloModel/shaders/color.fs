#version 330 core

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 lightColor;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
    float cutOff;
    float outerCutOff;
};

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform SpotLight spotLight;
uniform vec3 viewPos;

out vec4 FragColor;

void main()
{
    // ambient
    vec3 texture1 = texture(texture_diffuse1, TexCoords).rgb;
    vec3 ambient = spotLight.lightColor * texture1 * spotLight.ambient;
    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(spotLight.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = spotLight.lightColor * diff * texture1 * spotLight.diffuse;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 h = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, h), 0.0), 32.0);
    vec3 specular = spotLight.lightColor * spec * texture1 * spotLight.specular;

    // 衰减
    float distance = length(spotLight.position - FragPos);
    float attenuation = 1.0 / (spotLight.constant + spotLight.linear * distance +
                 spotLight.quadratic * (distance * distance));
    // spot light
    float theta = dot(lightDir, normalize(-spotLight.direction));
    float epsilon = spotLight.cutOff - spotLight.outerCutOff;
    float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 result  = (ambient + (diffuse + specular) * intensity * attenuation);

    FragColor = vec4(result, 1.0);
}

















