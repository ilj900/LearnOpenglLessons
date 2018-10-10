#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

uniform vec3 lightColor;
uniform vec3 cubeColor;
uniform float ambientStrength;
uniform vec3 lightPos;

out vec3 Color;
out vec3 fragPos;

void main()
{
    vec3 Normal = normalize(normalMatrix * aNorm);
    fragPos = vec3(view * model * vec4(aPos, 1.0));
    vec3 LightDir = normalize(lightPos - fragPos);
    float angleNormLight = dot(Normal, LightDir);

    //ambient
    float ambient = ambientStrength;

    //diffuse
    float diff = max(angleNormLight, 0.0);
    float diffuse = diff;

    //specular
    float specularStrength = 1.0;
    vec3 viewDir = normalize(-fragPos);
    float spec;
    if (angleNormLight <= 0.0)                                      //We have to calculate the angle between normal and light direction.
        spec = 0.0;                                                 //Or we'll have some false specular light, when the angle between reflected light direction
    else                                                            //and view direction is less that M_PI_2, though the angle between normal and
    {                                                               //light is more that M_PI_2, which means that fragment is not illuminated
        vec3 reflectDir = normalize(reflect(-LightDir, Normal));
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    }
    float specular = specularStrength * spec;

    Color = (ambient + diffuse + specular) * cubeColor * lightColor;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
