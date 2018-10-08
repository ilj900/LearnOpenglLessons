#version 330 core

uniform vec3 gridColor;

out vec4 FragColor;

void main()
{
    FragColor = vec4(gridColor, 1.0);
}
