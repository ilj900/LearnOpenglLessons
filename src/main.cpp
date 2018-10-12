#include <iostream>
#include <string>
#include <math.h>
#include "glad.h"
#include "glfw3.h"
#include "shader.h"
#include "camera.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void querryGlParams();
unsigned int loadTexture(const char * path);
glm::vec3 getColor();
GLFWmonitor* getMonitor();
camera *cam;
bool lightSourceMovement = true;

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

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!shaderManager::addShadervf("./res/shaders/object.vertex.shader", "./res/shaders/object.fragment.shader", "Simple Shader"))
        return -1;
    if (!shaderManager::addShadervf("./res/shaders/light.vertex.shader", "./res/shaders/light.fragment.shader", "Light Shader"))
        return -1;
    if (!shaderManager::addShadervf("./res/shaders/grid.vertex.shader", "./res/shaders/grid.fragment.shader", "Grid Shader"))
        return -1;

    cam = new camera(glm::vec3(0.0f, 2.5f, 10.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), M_PI_2/2.0f, 0.1f, 100.0f, 4.5f, (float)frameWidth, (float)frameHeight);

    glViewport(0, 0, frameWidth, frameHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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

    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
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

    glm::vec3 objectPosition(-1.0f, 0.5f, -1.0f);
    glm::vec3 lightPosition(-1.2f, 0.5f, 1.0f);
    glm::vec3 lightAmbient(0.2f, 0.2f, 0.2f);
    glm::vec3 lightDiffuse(0.5f, 0.5f, 0.5f);
    glm::vec3 lightSpecular(1.0f, 1.0f, 1.0f);
    float shininess = 64.0f;
    glm::vec3 gridColor(0.0f, 1.0f, 0.0f);
    static double deltaT = 0.0f;
    static double currentFrame = 0.0f;
    static double previousFrame = 0.0f;

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

        ///Some nasty rotations!!
        static double angle = 0.0f;
        if(lightSourceMovement)
            angle += deltaT;
        glm::vec3 relativeLightPos = lightPosition - objectPosition;
        glm::mat4 rotation(1.0f);
        rotation = glm::rotate(rotation, (float)angle, glm::vec3(0.0f, 1.0f, 0.0f));
        relativeLightPos = glm::vec3(rotation * glm::vec4(relativeLightPos, 0.0f));
        relativeLightPos = objectPosition + relativeLightPos;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderManager::setAndUse("Grid Shader");
        shaderManager::setMat4("model", glm::value_ptr(model));
        shaderManager::setMat4("view", glm::value_ptr(view));
        shaderManager::setMat4("projection", glm::value_ptr(projection));
        shaderManager::setVec3("gridColor", glm::value_ptr(gridColor));
        glBindVertexArray(gridVAO);
        glDrawArrays(GL_LINES, 0, 44);

        shaderManager::setAndUse("Simple Shader");
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureDiffuse);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textureSpecular);
        model = glm::mat4(1.0);
        model = glm::translate(model, objectPosition);
        ///Here might be a possible problem. I don't know how exactly glm creates 3x3 matrix from 4x4, hope it just takes an upper-left 3x3 from 4x4.
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(model));
        shaderManager::setMat4("model", glm::value_ptr(model));
        shaderManager::setMat4("view", glm::value_ptr(view));
        shaderManager::setMat4("projection", glm::value_ptr(projection));
        shaderManager::setVec3("viewPos", glm::value_ptr(camPos));
        shaderManager::setMat3("normalMatrix", glm::value_ptr(normalMatrix));
        shaderManager::setVec3("light.ambient", glm::value_ptr(lightAmbient));
        shaderManager::setVec3("light.diffuse", glm::value_ptr(lightDiffuse));
        shaderManager::setVec3("light.specular", glm::value_ptr(lightSpecular));
        shaderManager::setVec3("light.position", glm::value_ptr(relativeLightPos));
        shaderManager::setFloat("material.shininess", shininess);
        shaderManager::setInt("material.diffuse", 0);
        shaderManager::setInt("material.specular", 1);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        shaderManager::setAndUse("Light Shader");
        shaderManager::setMat4("view", glm::value_ptr(view));
        shaderManager::setMat4("projection", glm::value_ptr(projection));
        model = glm::mat4(1.0);
        model = glm::translate(model, relativeLightPos);
        model = glm::scale(model, glm::vec3(0.1f));
        shaderManager::setMat4("model", glm::value_ptr(model));
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
        Sleep(8);
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &gridVAO);
    glDeleteVertexArrays(1, &lightVAO);
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
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

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
    unsigned int params[] = {GL_MAX_VERTEX_ATTRIBS, GL_MAX_UNIFORM_BLOCK_SIZE};
    std::string explanations[] = {  "Maximum number of vertex attribures supported: ",
                                    "Maximum uniform block size supported: "};
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
        glTexImage2D(GL_TEXTURE_2D, 0, format, imgWidth, imgHeight, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINE);
    } else {
        std::cout<<"Failed to load diffuse map: "<<std::endl;
    }
    stbi_image_free(data);
    return texture;
}
