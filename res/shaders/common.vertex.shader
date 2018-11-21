#version 330 core

#define LIGHT_SHAPE     0
#define SPACE_GRID      1
#define FPS_GRAPH       2
#define SIMPLE_LINES    3

layout (location = 0) in vec2 incomingPosotion2D;       //For FPS graph
layout (location = 1) in vec3 incomingPosotion3D;       //For Space Grid and Light sources

uniform int targetFlag;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float offsetX;
uniform vec2 pos;
uniform float totalWidth;
uniform vec2 scale;

void main()
{
    switch(targetFlag)
    {
    case LIGHT_SHAPE:
        gl_Position = projection * view * model * vec4(incomingPosotion3D, 1.0);
        break;
    case FPS_GRAPH:
        vec2 screenPos = pos + vec2(mod(totalWidth - offsetX + incomingPosotion2D.x, totalWidth)*scale.x, incomingPosotion2D.y*scale.y);
        gl_Position = projection * vec4(screenPos, 0.0, 1.0);
        break;
    case SIMPLE_LINES:
        gl_Position = projection * vec4(incomingPosotion2D.x * scale.x, incomingPosotion2D.y*scale.y, 0.0, 1.0);
        break;
    case SPACE_GRID:
        gl_Position = projection * view * model * vec4(incomingPosotion3D, 1.0);
        break;
    }
}
