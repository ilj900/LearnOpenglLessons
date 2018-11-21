#version 330 core

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;

    float innerAngle;
    float outerAngle;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 4
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

in vec2 texCoords;
in vec3 Normal;
in vec3 fragPos;

uniform Material material;
uniform vec3 viewPos;
uniform bool spotlightSwitch;

out vec4 FragColor;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - fragPos);

    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, fragPos, viewDir);

    if (spotlightSwitch)
        result += CalcSpotLight(spotLight, norm, fragPos, viewDir);

    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    float angle = dot(normal, lightDir);
    float diff = max(angle, 0.0);
    float spec = 0.0;                                                       //We have to take into consideration the angle between normal and light direction.
    if (angle >= 0.0)                                                       //Or we'll have some false specular light, when the angle between reflected light direction
    {                                                                       //and view direction is less that M_PI_2, though the angle between normal and
        vec3 reflectDir = reflect(-lightDir, normal);                       //light is more that M_PI_2, which means that fragment is not illuminated
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    }
    vec3 tex = texture(material.diffuse, texCoords).rgb;
    vec3 ambient = light.ambient * tex;
    vec3 diffuse = light.diffuse * diff * tex;
    vec3 specular = light.specular * spec * tex;
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float angle = dot(normal, lightDir);
    float diff = max(angle, 0.0);
    float spec = 0.0;                                                       //We have to take into consideration the angle between normal and light direction.
    if (angle >= 0.0)                                                       //Or we'll have some false specular light, when the angle between reflected light direction
    {                                                                       //and view direction is less that M_PI_2, though the angle between normal and
        vec3 reflectDir = reflect(-lightDir, normal);                       //light is more that M_PI_2, which means that fragment is not illuminated
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    }

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    vec3 tex = texture(material.diffuse, texCoords).rgb;
    vec3 ambient = light.ambient * tex;
    vec3 diffuse = light.diffuse * diff * tex;
    vec3 specular = light.specular * spec * tex;
    return ((ambient + diffuse + specular) * attenuation);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float angle = dot(normal, lightDir);
    float diff = max(angle, 0.0);
    float spec = 0.0;                                                       //We have to take into consideration the angle between normal and light direction.
    if (angle >= 0.0)                                                       //Or we'll have some false specular light, when the angle between reflected light direction
    {                                                                       //and view direction is less that M_PI_2, though the angle between normal and
        vec3 reflectDir = reflect(-lightDir, normal);                       //light is more that M_PI_2, which means that fragment is not illuminated
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    }

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.innerAngle - light.outerAngle;
    float intensity = clamp((theta - light.outerAngle) / epsilon, 0.0, 1.0);

    vec3 tex = texture(material.diffuse, texCoords).rgb;
    vec3 ambient = light.ambient * tex;
    vec3 diffuse = light.diffuse * diff * tex * intensity;
    vec3 specular = light.specular * spec * tex * intensity;
    return ((ambient + diffuse + specular) * attenuation);
}

















