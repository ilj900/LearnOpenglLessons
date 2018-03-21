#include <iostream>
#include <string>
#include <math.h>
#include "glad.h"
#include "glfw3.h"
#include "shader.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void querryGlParams();

float vertices[] = {
     0.5,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
     0.0f,  0.5, 0.0f, 0.0f, 0.0f, 1.0f
};

unsigned int indices[] = {
    0, 1, 2
};

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

    if (!shaderManager::addShadervf("./res/shaders/vertex.shader", "./res/shaders/fragment.shader", "Simple Shader"))
        return -1;

    shaderManager::setCurrentShaderProgram("Simple Shader");

    glViewport(0, 0, 800, 600);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    unsigned int VBO, VAO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    float offset = 0.0f;
    float step = 0.005;

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        Sleep(16);

        glClear(GL_COLOR_BUFFER_BIT);
        shaderManager::use();
        glBindVertexArray(VAO);
        offset += step;
        if (offset > 0.5 || offset < -0.5)
            step = -step;
        shaderManager::setFloat("offsetX", offset);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void querryGlParams()
{
    unsigned int params[] = {GL_MAX_VERTEX_ATTRIBS, GL_MAX_UNIFORM_BLOCK_SIZE};
    std::string explanations[] = {  "Maximum number of vertex attribures supported: ",
                                    "Maximum uniform block size supported: "};
    for (unsigned int i = 0; i < sizeof(params)/sizeof(int); i++)
        shaderManager::querryGlParam(params[i], explanations[i]);

}
