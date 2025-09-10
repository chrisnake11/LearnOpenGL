#version 460 core

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
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 128.0); // shininess = 128
    
    // combine results
    vec3 result = (ambientStrength + diff + 0.5 * spec) * lightColor * objectColor;
    frag_color = vec4(result, 1.0);

}