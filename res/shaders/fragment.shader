#version 330 core

in vec2 TexCoord;

uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;

uniform bool drawLines;

out vec4 FragColor;

void main()
{
    if (drawLines == false)
        FragColor = mix(texture(ourTexture1, TexCoord), texture(ourTexture2, TexCoord), 0.2);
    else
        FragColor = vec4(0.0, 1.0, 0.0, 1.0);
}
