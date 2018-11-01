#include <iostream>
#include <string>
#include <math.h>
#include <map>
#include "glad.h"
#include "glfw3.h"
#include "shader.h"
#include "camera.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <ft2build.h>
#include FT_FREETYPE_H

typedef struct
{
    char ch;
    glm::ivec2 size;
    glm::ivec2 bearing;
    unsigned int advance;
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
    ///I'll use array of pointers to CHARACTER, for me it's simpler than std::map and faster;
    CHARACTER **characterPointerArray;
    unsigned int nmbrOfCharacters;
} CHARSET;

typedef struct
{
    std::string text;
    CHARSET *charset;
    unsigned int VAO;
    unsigned int VBO;
} TEXT;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);
void querryGlParams();
unsigned int loadTexture(const char * path);
void printOutTheSymbol(std::string filename, CHARACTER CH);
glm::vec3 getColor();
GLFWmonitor* getMonitor();
camera *cam;
bool lightSourceMovement = true;
FT_Library ft;
FT_Face face;

CHARSET* setupTreeType(const std::string typeFileName, unsigned int glyphSize, unsigned char nmbrOfGlyphs);
TEXT generateTextData(std::string text, CHARSET *charset);
void renderText(std::string shaderName, TEXT text, float xpos, float ypos, float scale, glm::vec3 color);

float vertices[] = {
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
    0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
    0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};

glm::vec3 cubePositions[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f),
    glm::vec3( 2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3( 2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3( 1.3f, -2.0f, -2.5f),
    glm::vec3( 1.5f,  2.0f, -2.5f),
    glm::vec3( 1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
};

glm::vec3 cubeRotaionAxises[] = {
    glm::vec3( 0.0f,  0.0f,  1.0f),
    glm::vec3( 1.0f,  0.0f, -1.0f),
    glm::vec3( 0.5f,  1.0f,  0.0f),
    glm::vec3(-1.0f, -1.0f,  0.0f),
    glm::vec3( 0.5f, -0.5f, -0.5f),
    glm::vec3(-0.7f,  0.0f,  0.5f),
    glm::vec3( 1.0f, -1.0f,  0.5f),
    glm::vec3( 1.0f,  0.1f, -0.5f),
    glm::vec3( 1.0f,  0.2f, -0.5f),
    glm::vec3(-1.0f,  1.0f,  0.5f)
};

float lineGrid[] =
{
    10.0f, 0.0f,  10.0f,  10.0f, 0.0f, -10.0f,
    8.0f,  0.0f,  10.0f,  8.0f,  0.0f, -10.0f,
    6.0f,  0.0f,  10.0f,  6.0f,  0.0f, -10.0f,
    4.0f,  0.0f,  10.0f,  4.0f,  0.0f, -10.0f,
    2.0f,  0.0f,  10.0f,  2.0f,  0.0f, -10.0f,
    0.0f,  0.0f,  10.0f,  0.0f,  0.0f, -10.0f,
    -2.0f,  0.0f,  10.0f, -2.0f,  0.0f, -10.0f,
    -4.0f,  0.0f,  10.0f, -4.0f,  0.0f, -10.0f,
    -6.0f,  0.0f,  10.0f, -6.0f,  0.0f, -10.0f,
    -8.0f,  0.0f,  10.0f, -8.0f,  0.0f, -10.0f,
    -10.0f, 0.0f,  10.0f, -10.0f, 0.0f, -10.0f,
    10.0f, 0.0f,  10.0f, -10.0f, 0.0f,  10.0f,
    10.0f, 0.0f,  8.0f,  -10.0f, 0.0f,  8.0f,
    10.0f, 0.0f,  6.0f,  -10.0f, 0.0f,  6.0f,
    10.0f, 0.0f,  4.0f,  -10.0f, 0.0f,  4.0f,
    10.0f, 0.0f,  2.0f,  -10.0f, 0.0f,  2.0f,
    10.0f, 0.0f,  0.0f,  -10.0f, 0.0f,  0.0f,
    10.0f, 0.0f, -2.0f,  -10.0f, 0.0f, -2.0f,
    10.0f, 0.0f, -4.0f,  -10.0f, 0.0f, -4.0f,
    10.0f, 0.0f, -6.0f,  -10.0f, 0.0f, -6.0f,
    10.0f, 0.0f, -8.0f,  -10.0f, 0.0f, -8.0f,
    10.0f, 0.0f, -10.0f, -10.0f, 0.0f, -10.0f
};

static int frameWidth = 800;
static int frameHeight = 600;
int monitor = 1;

int main()
{
    glfwInit();

    GLFWmonitor *bestMonitor = getMonitor();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(frameWidth, frameHeight, "LearnOpwnGL", bestMonitor, NULL);
    if (window == NULL)
    {
        const char *description;
        glfwGetError(&description);
        std::cout<<description<<std::endl;
        std::cout<<"Failed to create GLFW window"<<std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout <<"Failed to initialize GLAD"<<std::endl;
        return -1;
    }
    querryGlParams();
    GLint flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!shaderManager::addShadervf("./res/shaders/object.vertex.shader", "./res/shaders/object.fragment.shader", "Simple Shader"))
        return -1;
    if (!shaderManager::addShadervf("./res/shaders/grid.vertex.shader", "./res/shaders/grid.fragment.shader", "Grid Shader"))
        return -1;
    if (!shaderManager::addShadervf("./res/shaders/text.vertex.shader", "./res/shaders/text.fragment.shader", "Text Shader"))
        return -1;

    cam = new camera(glm::vec3(0.0f, 2.5f, 10.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), M_PI_2/2.0f, 0.1f, 100.0f, 4.5f, (float)frameWidth, (float)frameHeight);
    CHARSET *chrst = setupTreeType("fonts/arial.ttf", 128, 128);    /// I found out that symbols greater that 128 loads blank;

    glViewport(0, 0, frameWidth, frameHeight);
    glClearColor(0.2f, 0.4f, 0.8f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    unsigned int VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    unsigned int gridVBO, gridVAO;
    glGenBuffers(1, &gridVBO);
    glGenVertexArrays(1, &gridVAO);
    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lineGrid), lineGrid, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    unsigned int textureDiffuse = loadTexture("./res/textures/container2.png");
    unsigned int textureSpecular = loadTexture("./res/textures/container2_specular.png");

    glm::vec3 lightAmbient(0.2f, 0.2f, 0.2f);
    glm::vec3 lightDiffuse(0.5f, 0.5f, 0.5f);
    glm::vec3 lightSpecular(1.0f, 1.0f, 1.0f);
    float lightConstant = 1.0f;
    float lightLinear = 0.09;
    float lightQuadratic = 0.032f;
    float innerCutOff = glm::cos(glm::radians(12.5f));
    float outerCutOff = glm::cos(glm::radians(17.5f));
    float shininess = 128.0f;
    glm::vec3 gridColor(0.0f, 1.0f, 0.0f);
    static double deltaT = 0.0f;
    static double currentFrame = 0.0f;
    static double previousFrame = 0.0f;

    TEXT uppercaseEnglishLetters = generateTextData("ABCDEFGHIJKLMNOPQRSTUVWXYZ", chrst);
    TEXT lowercaseEnglishLetters = generateTextData("abcdefghijklmnopqrstuvwxyz", chrst);
    TEXT specialSymbols = generateTextData("1234567890-=!@#$%^&*()_", chrst);
    TEXT oneLetter = generateTextData("S", chrst);


    while (!glfwWindowShouldClose(window))
    {
        currentFrame = glfwGetTime();
        deltaT = currentFrame - previousFrame;
        previousFrame = currentFrame;

        processInput(window);

        glm::mat4 projection = cam->getProjection();
        glm::mat4 view = cam->getViewMatrix();
        glm::mat4 model(1.0f);
        glm::vec3 camPos = cam->getPosition();
        glm::vec3 camDir = cam->getDirection();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderManager::setAndUse("Grid Shader");
        shaderManager::setMat4("model", glm::value_ptr(model));
        shaderManager::setMat4("view", glm::value_ptr(view));
        shaderManager::setMat4("projection", glm::value_ptr(projection));
        shaderManager::setVec3("gridColor", glm::value_ptr(gridColor));
        glBindVertexArray(gridVAO);
        glDrawArrays(GL_LINES, 0, 44);

        shaderManager::setAndUse("Simple Shader");
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureDiffuse);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textureSpecular);
        shaderManager::setMat4("view", glm::value_ptr(view));
        shaderManager::setMat4("projection", glm::value_ptr(projection));
        shaderManager::setVec3("viewPos", glm::value_ptr(camPos));
        shaderManager::setVec3("light.position", glm::value_ptr(camPos));
        shaderManager::setVec3("light.direction", glm::value_ptr(camDir));
        shaderManager::setFloat("light.innerCutOff", innerCutOff);
        shaderManager::setFloat("light.outerCutOff", outerCutOff);
        shaderManager::setVec3("light.ambient", glm::value_ptr(lightAmbient));
        shaderManager::setVec3("light.diffuse", glm::value_ptr(lightDiffuse));
        shaderManager::setVec3("light.specular", glm::value_ptr(lightSpecular));
        shaderManager::setFloat("light.constant", lightConstant);
        shaderManager::setFloat("light.linear", lightLinear);
        shaderManager::setFloat("light.quadratic", lightQuadratic);
        shaderManager::setFloat("material.shininess", shininess);
        shaderManager::setInt("material.diffuse", 0);
        shaderManager::setInt("material.specular", 1);
        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < 10; i++)
        {
            model = glm::mat4(1.0);
            model = glm::translate(model, cubePositions[i]);
            model = glm::rotate(model, (float)glfwGetTime()*0.3f*(float)(i+1), cubeRotaionAxises[i]);
            shaderManager::setMat4("model", glm::value_ptr(model));
            glm::mat3 normalMatrix = glm::transpose(glm::inverse(model));
            shaderManager::setMat3("normalMatrix", glm::value_ptr(normalMatrix));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        renderText("Text Shader", uppercaseEnglishLetters, 50.0f, frameHeight*0.94f, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        renderText("Text Shader", lowercaseEnglishLetters, 50.0f, frameHeight*0.88f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        renderText("Text Shader", specialSymbols, 50.0f, frameHeight*0.82f, 1.0f, glm::vec3(0.0f, 0.0f, 1.0f));
        renderText("Text Shader", oneLetter, 1400.0, 208.0, 15.0, glm::vec3(1.0f, 0.0f, 0.0f));

        glfwSwapBuffers(window);
        glfwPollEvents();
        Sleep(8);
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &gridVAO);
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    frameWidth = width;
    frameHeight = height;
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    static float lastInput = 0.0f;
    static float currentInput = 0.0f;
    static float movementSpeed = 4.5f;

    currentInput = glfwGetTime();
    float deltaT = currentInput - lastInput;
    lastInput = currentInput;

    float front = 0.0f, up = 0.0f, right = 0.0f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        front += deltaT * movementSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        front -= deltaT * movementSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        right += deltaT * movementSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        right -= deltaT * movementSpeed;
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        up += deltaT * movementSpeed;
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        up -= deltaT * movementSpeed;
    cam->move(front, right, up);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        cam->rotate(0.0f, 0.0f, deltaT * movementSpeed*0.2f);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cam->rotate(0.0f, 0.0f, -deltaT * movementSpeed*0.2f);
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        cam->instantMove(glm::vec3(0.0f, 2.5f, 10.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        lightSourceMovement = !lightSourceMovement;
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    static double mouseSensitivity = 0.0005f;
    static double previosPosX = xpos;
    static double previosPosY = ypos;

    float deltax = (previosPosX - xpos)*mouseSensitivity;
    previosPosX = xpos;
    float deltay = (previosPosY - ypos)*mouseSensitivity;
    previosPosY = ypos;
    cam->rotate(deltax, deltay, 0.0f);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    cam->zoom(yoffset);
}

void querryGlParams()
{
    unsigned int params[] = {GL_MAX_VERTEX_ATTRIBS, GL_MAX_UNIFORM_BLOCK_SIZE, GL_MAX_TEXTURE_SIZE};
    std::string explanations[] = {  "Maximum number of vertex attribures supported: ",
                                    "Maximum uniform block size supported: ",
                                    "Maximum texture size is: "};
    for (unsigned int i = 0; i < sizeof(params)/sizeof(int); i++)
        shaderManager::querryGlParam(params[i], explanations[i]);

}

GLFWmonitor* getMonitor()
{
    int count;
    GLFWmonitor** monitors = glfwGetMonitors(&count);
    GLFWmonitor* chosenMonitor;
    if (monitor < count)
        chosenMonitor = monitors[monitor];
    else
        chosenMonitor = monitors[count-1];
    const GLFWvidmode *mode = glfwGetVideoMode(chosenMonitor);
    frameWidth = mode->width;
    frameHeight = mode->height;
    return chosenMonitor;
}

glm::vec3 getColor()
{
    float time = glfwGetTime();
    return glm::vec3(sin(time * 2.0f), sin(time * 1.3f), sin(time * 0.7f));
}

unsigned int loadTexture(const char * path)
{
    unsigned int texture;
    int imgWidth, imgHeight, nmbrOfChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(path, &imgWidth, &imgHeight, &nmbrOfChannels, 0);
    if (data)
    {
        GLenum format;
        switch(nmbrOfChannels)
        {
        case 1:
            format = GL_RED;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            std::cout<<"nmbrOfChannels was not 1, 3 or 4. Pull the string and push the window!"<<std::endl;
            exit(1);
        }

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, format, imgWidth, imgHeight, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout<<"Failed to load diffuse map: "<<std::endl;
    }
    stbi_image_free(data);
    return texture;
}

void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

        std::cout << "---------------" << std::endl;
        std::cout << "Debug message (" << id << "): " <<  message << std::endl;

        switch (source)
        {
            case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
            case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
            case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
            case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
            case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
        } std::cout << std::endl;

        switch (type)
        {
            case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
            case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
            case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
            case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
            case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
            case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
            case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
        } std::cout << std::endl;

        switch (severity)
        {
            case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
            case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
            case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
            case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
        } std::cout << std::endl;
        std::cout << std::endl;
}

CHARSET* setupTreeType(const std::string typeFileName, unsigned int glyphSize, unsigned char nmbrOfGlyphs)
{
    if (FT_Init_FreeType(&ft))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    if (FT_New_Face(ft, typeFileName.c_str(), 0, &face))
        std::cout << "ERROR::FREETYPE: Failed to load font from: " << typeFileName << std::endl;
    FT_Set_Pixel_Sizes(face, 0, glyphSize);

    CHARSET *charset = new CHARSET;
    unsigned int textureSize = 0;
    ///Allocate all the memory we might need, then we'll reallocate if necessary
    charset->characterPointerArray = (CHARACTER**)malloc(sizeof(CHARACTER**) * nmbrOfGlyphs);
    charset->nmbrOfCharacters = 0;

    ///Generate and store glypgs
    for (unsigned char c = 0; c < nmbrOfGlyphs; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph: " << c << std::endl;
            continue;
        }
        CHARACTER *ch = new CHARACTER;
        ch->ch = c;
        ch->size = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
        unsigned int bitmapSize = face->glyph->bitmap.width * face->glyph->bitmap.rows;
        ch->bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
        ch->advance = face->glyph->advance.x/64;    ///FT quote: "The advance vector is expressed in 1/64th of pixels, and is truncated to integer pixels on each iteration"
        ch->textureOffset = textureSize;
        ch->bitmapData = new char[bitmapSize];
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

    /// I'm gonna create a bitmap atlas for glyphs. I know there will be some free space, that's why FreeType leaves this job to users
    /// So. I want the resulting texture to be square, and I'm lazy to look for optimal algorythm hot wo place glyph bitmaps into atlas
    unsigned int totalGlyphArrayWidth = 0;
    for (unsigned int i = 0; i < charset->nmbrOfCharacters; i++)
        totalGlyphArrayWidth += charset->characterPointerArray[i]->size.x;


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
    }

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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    return charset;
}

TEXT generateTextData(std::string text, CHARSET *charset)
{
    TEXT txt;
    txt.text = text;
    txt.charset = charset;

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
    charVertice *buffer = new charVertice[6 * text.size()];
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
        currentAdvance+=ch->advance;
    }

    glGenVertexArrays(1, &txt.VAO);
    glGenBuffers(1, &txt.VBO);
    glBindVertexArray(txt.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, txt.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(charVertice) * 6 * text.size(), buffer, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(charVertice), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(charVertice), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    return txt;
}

void renderText(std::string shaderName, TEXT text, float xpos, float ypos, float scale, glm::vec3 color)
{
    glm::vec2 pos(xpos, ypos);
    glBindVertexArray(text.VAO);
    shaderManager::setAndUse(shaderName);
    shaderManager::setVec2("pos", glm::value_ptr(pos));
    shaderManager::setFloat("scale", scale);
    shaderManager::setVec3("textColor", glm::value_ptr(color));
    glm::mat4 textProjection = glm::ortho(0.0f, (float)frameWidth, 0.0f, (float)frameHeight);
    shaderManager::setMat4("projection", glm::value_ptr(textProjection));
    shaderManager::setInt("text", 0);
    shaderManager::setFloat("totalTextureW", (float)text.charset->texture.textureW);
    shaderManager::setFloat("totalTextureH", (float)text.charset->texture.textureH);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, text.charset->texture.textureID);
    glDrawArrays(GL_TRIANGLES, 0, 6*text.text.length());
    glBindVertexArray(0);
}

void printOutTheSymbol(std::string filename, CHARACTER CH)
{

}


