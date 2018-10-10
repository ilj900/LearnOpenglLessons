#version 330 core

in vec3 Normal;
in vec3 fragPos;

uniform vec3 lightColor;
uniform vec3 cubeColor;
uniform float ambientStrength;
uniform vec3 lightPos;

out vec4 FragColor;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 LightDir = normalize(lightPos - fragPos);
    float angleNormLight = dot(norm, LightDir);

    //ambient
    vec3 ambient = lightColor * ambientStrength;

    //diffuse
    float diff = max(angleNormLight, 0.0);
    vec3 diffuse = diff * lightColor;

    //specular
    float specularStrength = 1.0;
    vec3 viewDir = normalize(-fragPos);
    float spec;
    if (angleNormLight <= 0.0)                                 //We have to calculate the angle between normal and light direction.
        spec = 0.0;                                                 //Or we'll have some false specular light, when the angle between reflected light direction
    else                                                            //and view direction is less that M_PI_2, though the angle between normal and
    {                                                               //light is more that M_PI_2, which means that fragment is not illuminated
        vec3 reflectDir = normalize(reflect(-LightDir, norm));
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    }
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result;
    result = (ambient + diffuse + specular) * cubeColor;

    FragColor = vec4(result, 1.0);
}
