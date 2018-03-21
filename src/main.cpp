#include <iostream>
#include <fstream>
#include <string>
#include "glad.h"
#include "glfw3.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

int readFromFile(const std::string path, char **src, int *size);

float vertices[] = {
     0.5f,  0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
    -0.5f,  0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
    -0.5f,  0.5f, 0.0f
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

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    char *vSrc, *fSrcRed, *fSrcYellow;
    int size;
    if(readFromFile("./res/shaders/vertex.shader", &vSrc, &size) == 0)
        return -1;
    if(readFromFile("./res/shaders/fragment_red.shader", &fSrcRed, &size) == 0)
        return -1;
    if(readFromFile("./res/shaders/fragment_yellow.shader", &fSrcYellow, &size) == 0)
        return -1;

    unsigned int vertexShader, fragmentShaderRed, fragmentShaderYellow;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShaderRed = glCreateShader(GL_FRAGMENT_SHADER);
    fragmentShaderYellow = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertexShader, 1, &vSrc, NULL);
    glCompileShader(vertexShader);
    delete vSrc;

    glShaderSource(fragmentShaderRed, 1, &fSrcRed, NULL);
    glCompileShader(fragmentShaderRed);
    delete fSrcRed;

    glShaderSource(fragmentShaderYellow, 1, &fSrcYellow, NULL);
    glCompileShader(fragmentShaderYellow);
    delete fSrcYellow;

    unsigned int programR = glCreateProgram();
    unsigned int programY = glCreateProgram();
    glAttachShader(programR, vertexShader);
    glAttachShader(programR, fragmentShaderRed);
    glLinkProgram(programR);

    glAttachShader(programY, vertexShader);
    glAttachShader(programY, fragmentShaderYellow);
    glLinkProgram(programY);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShaderRed);
    glDeleteShader(fragmentShaderYellow);

    glViewport(0, 0, 800, 600);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    glUseProgram(programR);
    unsigned int VBO1, VAO1;
    glGenBuffers(1, &VBO1);
    glGenVertexArrays(1, &VAO1);
    glBindVertexArray(VAO1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, 9*sizeof(float), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glUseProgram(programY);
    unsigned int VBO2, VAO2;
    glGenBuffers(1, &VBO2);
    glGenVertexArrays(1, &VAO2);
    glBindVertexArray(VAO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, 9*sizeof(float), &vertices[9], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(programR);
        glBindVertexArray(VAO1);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glUseProgram(programY);
        glBindVertexArray(VAO2);
        glDrawArrays(GL_TRIANGLES, 0, 3);

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

int readFromFile(const std::string path, char** src, int *size)
{
    std::ifstream in(path.c_str(), std::ios::in | std::ios::binary);
    if (in)
    {
        in.seekg(0, std::ios::end);
        int l_size = in.tellg();
        in.seekg(0, std::ios::beg);
        char* newSrc = new char[l_size+1];
        in.read(newSrc, l_size);
        in.close();
        newSrc[l_size] = '\0';
        *size = l_size;
        *src = newSrc;
        return l_size;
    }
    return 0;
}
