#ifndef MAIN_H
#define MAIN_H

float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
    0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
    // Front face
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
    // Left face
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
    // Right face
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
    0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
    0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
    0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
    // Bottom face
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
    0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
    // Top face
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left
};

float thrd = 0.33333333;

float screenQuads[] = {
    -1.0f, -1.0f, 0.16666666f, 0.0f,
    -thrd, -1.0f, 0.83333333f, 0.0f,
    -1.0f,  0.0f, 0.16666666f, 1.0f,
    -1.0f,  0.0f, 0.16666666f, 1.0f,
    -thrd, -1.0f, 0.83333333f, 0.0f,
    -thrd,  0.0f, 0.83333333f, 1.0f,

    -thrd, -1.0f, 0.16666666f, 0.0f,
     thrd, -1.0f, 0.83333333f, 0.0f,
    -thrd,  0.0f, 0.16666666f, 1.0f,
    -thrd,  0.0f, 0.16666666f, 1.0f,
     thrd, -1.0f, 0.83333333f, 0.0f,
     thrd,  0.0f, 0.83333333f, 1.0f,

     thrd, -1.0f, 0.16666666f, 0.0f,
     1.0f, -1.0f, 0.83333333f, 0.0f,
     thrd,  0.0f, 0.16666666f, 1.0f,
     thrd,  0.0f, 0.16666666f, 1.0f,
     1.0f, -1.0f, 0.83333333f, 0.0f,
     1.0f,  0.0f, 0.83333333f, 1.0f,

    -1.0f,  0.0f, 0.16666666f, 0.0f,
    -thrd,  0.0f, 0.83333333f, 0.0f,
    -1.0f,  1.0f, 0.16666666f, 1.0f,
    -1.0f,  1.0f, 0.16666666f, 1.0f,
    -thrd,  0.0f, 0.83333333f, 0.0f,
    -thrd,  1.0f, 0.83333333f, 1.0f,

    -thrd,  0.0f, 0.16666666f, 0.0f,
     thrd,  0.0f, 0.83333333f, 0.0f,
    -thrd,  1.0f, 0.16666666f, 1.0f,
    -thrd,  1.0f, 0.16666666f, 1.0f,
     thrd,  0.0f, 0.83333333f, 0.0f,
     thrd,  1.0f, 0.83333333f, 1.0f,

     thrd,  0.0f, 0.16666666f, 0.0f,
     1.0f,  0.0f, 0.83333333f, 0.0f,
     thrd,  1.0f, 0.16666666f, 1.0f,
     thrd,  1.0f, 0.16666666f, 1.0f,
     1.0f,  0.0f, 0.83333333f, 0.0f,
     1.0f,  1.0f, 0.83333333f, 1.0f
};

typedef struct
{
    unsigned int framebufferID;
    unsigned int textureAttachemntID;
    unsigned int renderBufferID;    //For depth and stencil attachments
} framebuffer;

#define MAIN_MONITOR 0
#define SECOND_MONITOR 1

#define LIGHT_SHAPE     0
#define SPACE_GRID      1
#define FPS_GRAPH       2
#define SIMPLE_LINES    3

#endif
