#version 330 core

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

in vec2 texCoords;
in vec3 Normal;
in vec3 fragPos;

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

out vec4 FragColor;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 tex = texture(material.diffuse, texCoords).rgb;
    vec3 LightDir = normalize(light.position - fragPos);
    float angle = dot(norm, LightDir);

    //ambient
    vec3 ambient = light.ambient * tex;

    //diffuse
    float diff = max(angle, 0.0);
    vec3 diffuse = light.diffuse * diff * tex;

    //specular
    vec3 viewDir = normalize(viewPos - fragPos);
    float spec;
    if (angle <= 0.0)                                                       //We have to calculate the angle between normal and light direction.
        spec = 0.0;                                                         //Or we'll have some false specular light, when the angle between reflected light direction
    else                                                                    //and view direction is less that M_PI_2, though the angle between normal and
    {                                                                       //light is more that M_PI_2, which means that fragment is not illuminated
        vec3 reflectDir = normalize(reflect(-LightDir, norm));
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    }
    vec3 specular = spec * light.specular * texture(material.specular, texCoords).rgb;

    float distance = length(light.position - fragPos);
    float attenuation = 1.0/(light.constant + light.linear * distance + light.quadratic * distance * distance);

    vec3 result;
    result = (diffuse + ambient + specular) * attenuation;

    FragColor = vec4(result, 1.0);
}
