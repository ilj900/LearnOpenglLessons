#ifndef TEXTOUTPUT_H
#define TEXTOUTPUT_H

#include <string>
#include <glm/glm.hpp>

typedef struct
{
    char ch;
    glm::ivec2 size;
    glm::ivec2 bearing;
    glm::ivec2 advance;
    unsigned int textureOffset;
    char *bitmapData;
} CHARACTER;

typedef struct
{
    unsigned int textureID;
    unsigned int textureSize;
    unsigned int textureW;
    unsigned int textureH;
} TEXTURE;

typedef struct
{
    TEXTURE texture;
    ///I'll use array of pointers to CHARACTER, for me it's simpler than std::map, and faster;
    CHARACTER **characterPointerArray;
    unsigned int nmbrOfCharacters;
} CHARSET;

typedef struct
{
    std::string text;
    CHARSET *charset;
    unsigned int VAO;
    unsigned int VBO;
    int width;
    int height;
} TEXT;

CHARSET* setupTreeType(const std::string typeFileName, unsigned int glyphSize, unsigned char nmbrOfGlyphs);
TEXT* generateTextData(std::string text, CHARSET *charset, int additionalHeightInterval);
void renderText(std::string shaderName, TEXT* text, float xpos, float ypos, float scale, glm::vec3 color, int frameWidth, int frameHeight);

#endif
