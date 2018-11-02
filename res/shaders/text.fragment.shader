#version 330 core
noperspective in vec4 TexGlypgCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;
uniform float totalTextureH;
uniform float totalTextureW;

void main()
{
    float textureCoord = TexGlypgCoords.w + TexGlypgCoords.z*trunc(TexGlypgCoords.y) + trunc(TexGlypgCoords.x);
    float pixely = (trunc(textureCoord/totalTextureW) + 0.5)/totalTextureH;
    float pixelx = (mod(textureCoord, totalTextureW) +0.5)/totalTextureW;
    color = vec4(textColor, texture(text, vec2(pixelx, pixely)).r);
}
