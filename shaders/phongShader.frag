#version 410 core

vec3 lightPos = vec3(0.f, 10.f, 0.f);

uniform vec3 cameraPos;

in vec3 worldPos;
in vec3 normal;

out vec4 FragColor;

void main()
{
    const vec3 ambientColor = vec3(1.f, 1.f, 1.f);
    const vec3 objectColor = vec3(0.15f, 0.5f, 0.87f);
    const vec3 lightColor = vec3(1.f, 1.f, 1.f);

    // ambient
    const float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - worldPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(cameraPos - worldPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}
