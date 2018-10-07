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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void querryGlParams();
camera *cam;

float vertices[] = {
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
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

int main()
{

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpwnGL", NULL, NULL);
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!shaderManager::addShadervf("./res/shaders/object.vertex.shader", "./res/shaders/object.fragment.shader", "Simple Shader"))
        return -1;
    if (!shaderManager::addShadervf("./res/shaders/light.vertex.shader", "./res/shaders/light.fragment.shader", "Light Shader"))
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
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

    glm::vec3 objectsColor(1.0f, 0.5f, 0.31f);
    glm::vec3 objectPosition(-1.0f, 1.0f, -1.0f);
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
    glm::vec3 lightPosition(1.2f, 1.0f, 2.0f);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        Sleep(16);

        glm::mat4 projection = cam->getProjection();
        glm::mat4 view = cam->getViewMatrix();
        glm::mat4 model(1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderManager::setCurrentShaderProgram("Simple Shader");
        shaderManager::use();
        model = glm::translate(model, objectPosition);
        shaderManager::setMat4("view", glm::value_ptr(view));
        shaderManager::setMat4("projection", glm::value_ptr(projection));
        shaderManager::setBool("drawLines", false);
        shaderManager::setVec3("cubeColor", glm::value_ptr(objectsColor));
        shaderManager::setVec3("lightColor", glm::value_ptr(lightColor));
        shaderManager::setMat4("model", glm::value_ptr(model));
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(gridVAO);
        shaderManager::setBool("drawLines", true);
        glDrawArrays(GL_LINES, 0, 44);

        shaderManager::setCurrentShaderProgram("Light Shader");
        shaderManager::use();
        shaderManager::setMat4("view", glm::value_ptr(view));
        shaderManager::setMat4("projection", glm::value_ptr(projection));
        shaderManager::setVec3("lightColor", glm::value_ptr(lightColor));
        model = glm::translate(model, lightPosition);
        model = glm::scale(model, glm::vec3(0.1f));
        shaderManager::setMat4("model", glm::value_ptr(model));
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
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
        cam->rotate(0.0f, 0.0f, 0.01f);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cam->rotate(0.0f, 0.0f, -0.01f);
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        cam->instantMove(glm::vec3(0.0f, 2.5f, 10.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
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
