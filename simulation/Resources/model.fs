#version 330 core
in vec3 FragPos;
in vec3 Normal;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;

uniform Material material;
uniform Light light;

void main() 
{
    // 环境光
    vec3 ambient = light.ambient * material.ambient;

    // 漫反射
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    // 镜面反射 (Blinn-Phong)
    vec3 viewDir = normalize(vec3(0.0, 0.0, 10.0) - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir),0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}