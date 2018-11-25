#include <ft2build.h>
#include FT_FREETYPE_H

#include <textOutput.h>
#include <glad.h>
#include <iostream>
#include <shader.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

FT_Library ft;
FT_Face face;

CHARSET* setupTreeType(const std::string typeFileName, unsigned int glyphSize, unsigned char nmbrOfGlyphs)
{
    if (FT_Init_FreeType(&ft))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    if (FT_New_Face(ft, typeFileName.c_str(), 0, &face))
        std::cout << "ERROR::FREETYPE: Failed to load font from: " << typeFileName << std::endl;
    FT_Set_Pixel_Sizes(face, 0, glyphSize);

    CHARSET *charset = new CHARSET();
    unsigned int textureSize = 0;
    ///Allocate all the memory we might need, then we'll reallocate if necessary
    charset->characterPointerArray = new CHARACTER*[nmbrOfGlyphs]();
    charset->nmbrOfCharacters = 0;

    ///Generate and store glypgs
    for (unsigned char c = 0; c < nmbrOfGlyphs; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph: " << c << std::endl;
            continue;
        }
        CHARACTER *ch = new CHARACTER();
        ch->ch = c;
        ch->size = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
        unsigned int bitmapSize = face->glyph->bitmap.width * face->glyph->bitmap.rows;
        ch->bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
        ch->advance.x = face->glyph->advance.x/64;    ///FT quote: "The advance vector is expressed in 1/64th of pixels, and is truncated to integer pixels on each iteration"
        ch->advance.y = face->glyph->bitmap.rows;       ///Here should be another value, but FT doesn't generated vertical advance for that type
        ch->textureOffset = textureSize;
        ch->bitmapData = new char[bitmapSize]();
        memcpy(ch->bitmapData, face->glyph->bitmap.buffer, bitmapSize);
        textureSize += bitmapSize;
        charset->characterPointerArray[charset->nmbrOfCharacters] = ch;
        charset->nmbrOfCharacters++;
    }
    if (charset->nmbrOfCharacters != nmbrOfGlyphs)
    {
        std::cout<<"Freeing excessive memory"<<std::endl;
        ///Free excessive allocated memory here
    }

    /// Assemble bitmap atlas and generate, load and setup texture
    /// Here I have a textureSize, at first I tried to factorize it to get width and height, but soon I found out
    /// that I have a MAX_TEXTURE_SIZE, and resulting W or H can be a prime number bigger that MAX_TEXTURE_SIZE;
    /// So I'll just create a square texture little bigger that I need.
    unsigned int textureSideLength = (unsigned int)(sqrt((double)textureSize))+1;
    char *texture = new char[textureSideLength*textureSideLength]();
    charset->texture.textureSize = textureSideLength*textureSideLength;

    for(char c = 0; c < charset->nmbrOfCharacters; c++)
    {
        CHARACTER *currentChar = charset->characterPointerArray[c];
        memcpy(&texture[currentChar->textureOffset], currentChar->bitmapData, currentChar->size.x*currentChar->size.y);
        delete[] currentChar->bitmapData;
    }

    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &charset->texture.textureID);
    glBindTexture(GL_TEXTURE_2D, charset->texture.textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, textureSideLength, textureSideLength, 0, GL_RED, GL_UNSIGNED_BYTE, texture);
    glBindTexture(GL_TEXTURE_2D, 0);
    charset->texture.textureW = textureSideLength;
    charset->texture.textureH =textureSideLength;

    delete[] texture;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    return charset;
}

TEXT* generateTextData(std::string text, CHARSET *charset, int additionalHeightInterval)
{
    TEXT* txt = new TEXT;
    txt->text = text;
    txt->charset = charset;

    struct charVertice
    {
        float posx;
        float posy;
        float texturex;
        float texturey;
        float texturewidth;
        float textureoffset;
    };

    float currentAdvance = 0.0;
    float maxH = 0.0;
    charVertice *buffer = new charVertice[6 * text.size()]();
    unsigned int counter = 0;

    for (unsigned int i = 0; i < text.size(); i++)
    {
        CHARACTER *ch;
        for(unsigned int j = 0; j < charset->nmbrOfCharacters; j++)
        {
            if (charset->characterPointerArray[j]->ch == text[i])
            {
                ch = charset->characterPointerArray[j];
                break;
            }
            if (j == charset->nmbrOfCharacters)
            {
                std::cout<<"I didn't find symbol \""<<text[i]<<"\" in my alphabet"<<std::endl;
                exit(3);
            }
        }
        float xposleft = currentAdvance + ch->bearing.x;
        float xposRight = xposleft + ch->size.x;
        float yposbot = ch->bearing.y - ch->size.y;
        float ypostop = yposbot + ch->size.y;
        float textureOffset = ch->textureOffset;
        float textureW = ch->size.x;
        float textureH = ch->size.y;

        charVertice lefttop =   {xposleft,  ypostop, 0.0f,        0.0f,     textureW, textureOffset};
        charVertice leftbot =   {xposleft,  yposbot, 0.0f,        textureH, textureW, textureOffset};
        charVertice rightTop =  {xposRight, ypostop, textureW,    0.0f,     textureW, textureOffset};
        charVertice rightBot =  {xposRight, yposbot, textureW,    textureH, textureW, textureOffset};

        memcpy(&buffer[counter], &leftbot, sizeof(charVertice));
        memcpy(&buffer[counter+1], &lefttop, sizeof(charVertice));
        memcpy(&buffer[counter+2], &rightBot, sizeof(charVertice));
        memcpy(&buffer[counter+3], &lefttop, sizeof(charVertice));
        memcpy(&buffer[counter+4], &rightTop, sizeof(charVertice));
        memcpy(&buffer[counter+5], &rightBot, sizeof(charVertice));

        counter+=6;
        currentAdvance+=ch->advance.x;
        if (maxH < ch->advance.y)
            maxH = ch->advance.y;
    }
    txt->height = maxH + additionalHeightInterval;
    txt->width = currentAdvance;

    glGenVertexArrays(1, &txt->VAO);
    glGenBuffers(1, &txt->VBO);
    glBindVertexArray(txt->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, txt->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(charVertice) * 6 * text.size(), buffer, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(charVertice), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(charVertice), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    delete[] buffer;
    return txt;
}

void renderText(std::string shaderName, TEXT* text, float xpos, float ypos, float scale, glm::vec3 color, int frameWidth, int frameHeight)
{
    glm::vec2 pos(xpos, ypos);
    glBindVertexArray(text->VAO);
    shaderManager::setAndUse(shaderName);
    shaderManager::setVec2("pos", glm::value_ptr(pos));
    shaderManager::setFloat("scale", scale);
    shaderManager::setVec3("textColor", glm::value_ptr(color));
    glm::mat4 textProjection = glm::ortho(0.0f, (float)frameWidth, 0.0f, (float)frameHeight);
    shaderManager::setMat4("projection", glm::value_ptr(textProjection));
    shaderManager::setInt("text", 0);
    shaderManager::setFloat("totalTextureW", (float)text->charset->texture.textureW);
    shaderManager::setFloat("totalTextureH", (float)text->charset->texture.textureH);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, text->charset->texture.textureID);
    glDrawArrays(GL_TRIANGLES, 0, 6*text->text.length());
    glBindVertexArray(0);
}
