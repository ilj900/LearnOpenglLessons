#version 330 core
layout (location = 0) in vec2 vertex;

uniform mat4 projection;
uniform float offsetX;
uniform vec2 pos;
uniform float totalWidth;
uniform vec2 scale;
uniform bool drawGraph;

void main()
{
    if(drawGraph)
    {
        vec2 screenPos = pos + vec2(mod(totalWidth - offsetX + vertex.x, totalWidth)*scale.x, vertex.y*scale.y);
        gl_Position = projection * vec4(screenPos, 0.0, 1.0);
    } else {
        gl_Position = projection * vec4(vertex.x * totalWidth, vertex.y, 0.0, 1.0);
    }
}
