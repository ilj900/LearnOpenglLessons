#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

uniform bool backgroundDrawing;

void main()
{
    if (backgroundDrawing)
        FragColor = vec4(0.04, 0.28, 0.26, 1.0);
    else
        FragColor = texture(texture_diffuse1, TexCoords);
}
