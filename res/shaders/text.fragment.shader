#version 330 core
in vec4 TexGlypgCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;
uniform float totalTextureH;
uniform float totalTextureW;

void main()
{
    float textureCoord = trunc(TexGlypgCoords.w +  TexGlypgCoords.z*trunc(TexGlypgCoords.y) + trunc(TexGlypgCoords.x));
    float pixely = trunc(textureCoord/totalTextureW)/totalTextureH;
    float pixelx = mod(textureCoord, totalTextureW)/totalTextureW;
    color = vec4(textColor, texture(text, vec2(pixelx, pixely)).r);
     //color = vec4(pixelx, 0.0, 0.0, 1.0);
}
