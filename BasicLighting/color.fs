#version 460 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

uniform Material material;

in vec3 Normal;
in vec3 FragPos;

out vec4 frag_color;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform float ambientStrength;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    // normalized vectors
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);

    // the angle cosine equals to the dot product of normalized vectors
    float diff = max(dot(norm, lightDir), 0.0);
    
    // Blinn-Phong specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);

    // combine results
    vec3 result = (material.ambient + diff * material.diffuse + spec * material.specular) * lightColor * objectColor;
    frag_color = vec4(result, 1.0);

}