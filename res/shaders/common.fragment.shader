#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;
uniform bool drawGrass;

void main()
{
    if (drawGrass)
    {
        vec4 texColor = texture(texture1, TexCoords);
        if(texColor.a < 0.01)
            discard;
        FragColor = texColor;
    } else {
        FragColor = texture(texture1, TexCoords);
    }
}
