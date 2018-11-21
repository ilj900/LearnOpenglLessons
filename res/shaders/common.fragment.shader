#version 330 core

#define LIGHT_SHAPE     0
#define SPACE_GRID      1
#define FPS_GRAPH       2
#define SIMPLE_LINES    3

uniform int targetFlag;

uniform vec3 generalColor;

out vec4 FragColor;

void main()
{
    switch(targetFlag)
    {
    case LIGHT_SHAPE:
        FragColor = vec4(generalColor, 1.0);
        break;
    case SPACE_GRID:
        FragColor = vec4(generalColor, 1.0);
        break;
    case FPS_GRAPH:
        FragColor = vec4(generalColor, 1.0);
        break;
    case SIMPLE_LINES:
        FragColor = vec4(generalColor, 1.0);
        break;
    }
}
