#version 330 core

in vec3 Normal;
in vec3 fragPos;

uniform vec3 lightColor;
uniform vec3 cubeColor;
uniform float ambientStrength;
uniform vec3 lightPos;
uniform vec3 viewPos;

out vec4 FragColor;

void main()
{
    //diffuse
    vec3 norm = normalize(Normal);
    vec3 LightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, LightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    //ambient
    vec3 ambient = lightColor * ambientStrength;

    //specular

    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-LightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * cubeColor;
    FragColor = vec4(result, 1.0);
}
