#version 330 core

uniform bool drawLines;
uniform vec3 lightColor;
uniform vec3 cubeColor;

out vec4 FragColor;

void main()
{
    if (drawLines == false)
        FragColor = vec4(cubeColor*lightColor, 1.0);
    else
        FragColor = vec4(0.0, 1.0, 0.0, 1.0);
}
