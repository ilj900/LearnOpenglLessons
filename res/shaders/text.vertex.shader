#version 330 core
layout (location = 0) in vec2 vertex;
layout (location = 1) in vec4 texture;

noperspective out vec4 TexGlypgCoords;

uniform mat4 projection;
uniform vec2 pos;
uniform float scale;

void main()
{
    gl_Position = projection * vec4(pos + (vertex.xy*scale), 0.0, 1.0);
    TexGlypgCoords = texture;
}
