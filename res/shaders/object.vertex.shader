#version 330 core

layout (location = 0) in vec3 aPos;

uniform bool drawLines;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    if (drawLines == false)
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    else
        gl_Position = projection * view * vec4(aPos, 1.0);
}
