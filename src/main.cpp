#include <iostream>
#include <string>
#include <math.h>
#include <map>
#include <glad.h>
#include <glfw3.h>
#include <shader.h>
#include <camera.h>
#include <textOutput.h>
#include <model.h>
#include <main.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);
framebuffer* createFramebuffer(unsigned int nrOfSamplers, unsigned int width, unsigned height);
float getRandom(float left, float right);
void querryGlParams();
GLFWmonitor* getMonitor();
camera* cam;

static int frameWidth = 800;
static int frameHeight = 600;
static int monitor = SECOND_MONITOR;
static bool vSync = true;
static float desiredFrameLength = 1000.0f/60.0f;
static unsigned int nrOfSamplers = 4;

int main()
{
    glfwInit();

    GLFWmonitor *bestMonitor = getMonitor();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    if (nrOfSamplers > 1)
        glfwWindowHint(GLFW_SAMPLES, nrOfSamplers);
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

    if (!shaderManager::addShadervf("./res/shaders/model.vertex.shader", "./res/shaders/model.fragment.shader", "Model Shader"))
        return -1;
    if (!shaderManager::addShadervf("./res/shaders/simple.vertex.shader", "./res/shaders/simple.fragment.shader", "Simple Shader"))
        return -1;

    cam = new camera(glm::vec3(0.0f, 0.5f, 50.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), M_PI_2/2.0f, 0.1f, 10000.0f, 7.5f, (float)frameWidth, (float)frameHeight, YAW_ROLL_PITCH);

    glViewport(0, 0, frameWidth, frameHeight);
    glEnable(GL_CULL_FACE);
    if (nrOfSamplers > 1)
        glEnable(GL_MULTISAMPLE);

    Model planet("J:/Workspace/Models/planet/planet.obj");
    Model asteroid("J:/Workspace/Models/rock/rock.obj");

    unsigned int nrOfAsteroids = 100000;
    glm::mat4 *modelMatreces = new glm::mat4[nrOfAsteroids];
    srand(glfwGetTime());
    float radius = 100.0f;
    float width = 40.0f;
    float height = 1.0f;
    for(unsigned int i = 0; i < nrOfAsteroids; i++)
    {
        glm::mat4 model(1.0f);

        float xOffset;
        if(i%7 == 0)
            xOffset = getRandom(-width*0.75, width*0.75);
        if(i%9 == 0)
            xOffset = getRandom(-width*0.8125, width*0.8125);
        else if (i%11 == 0)
            xOffset = getRandom(-width*0.875, width*0.875);
        else if (i%13 == 0)
            xOffset = getRandom(-width, width);
        else if (i%17 == 0)
            xOffset = getRandom(-width*1.125, width*1.125);
        else if (i%19 == 0)
            xOffset = getRandom(-width*1.25, width*1.25);
        float yOffset = getRandom(-height, height);
        float anglePlanet = getRandom(0.0f, 360.0f);
        float angleSelf = getRandom(0.0f, 360.0f);
        float scale = getRandom(0.05, 0.25);

        model = glm::rotate(model, anglePlanet, glm::vec3(0.0, 1.0, 0.0));
        model = glm::translate(model, glm::vec3(radius + xOffset, yOffset, 0.0));
        model = glm::rotate(model, angleSelf, glm::vec3(0.4f, 0.6f, 0.8f));
        model = glm::scale(model, glm::vec3(scale));

        modelMatreces[i] = model;
    }

    unsigned int modelBuffer;
    glGenBuffers(1, &modelBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, modelBuffer);
    glBufferData(GL_ARRAY_BUFFER, nrOfAsteroids * sizeof(glm::mat4), &modelMatreces[0], GL_STATIC_DRAW);

    for (unsigned int i = 0; i < asteroid.meshes.size(); i++)
    {
        unsigned int VAO = asteroid.meshes[i].VAO;
        glBindVertexArray(VAO);
        GLsizei vec4Size = sizeof(glm::vec4);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
        glBindVertexArray(0);
    }

    framebuffer *fbms = createFramebuffer(nrOfSamplers, frameWidth, frameHeight);
    framebuffer *fb = createFramebuffer(1, frameWidth, frameHeight);

    unsigned int frontSquareVBO, frontSquareVAO;
    glGenVertexArrays(1, &frontSquareVAO);
    glGenBuffers(1, &frontSquareVBO);
    glBindVertexArray(frontSquareVAO);
    glBindBuffer(GL_ARRAY_BUFFER, frontSquareVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(frontSquare), frontSquare, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) (2 * sizeof(float)));
    glBindVertexArray(0);

    static float deltaT = 0.0f;
    static float currentFrame = 0.0f;
    static float previousFrame = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        currentFrame = glfwGetTime();
        deltaT = currentFrame - previousFrame;
        previousFrame = currentFrame;

        processInput(window);

        glm::mat4 projection = cam->getProjection();;
        glm::mat4 view = cam->getViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        glBindFramebuffer(GL_FRAMEBUFFER, fbms->framebufferID);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderManager::setAndUse("Model Shader");
        shaderManager::setInt("drawObject", 1);
        shaderManager::setMat4("view", glm::value_ptr(view));
        shaderManager::setMat4("projection", glm::value_ptr(projection));
        model = glm::translate(model, glm::vec3(0.0, -8.0, 0.0));
        model = glm::scale(model, glm::vec3(10));
        shaderManager::setMat4("model", glm::value_ptr(model));
        asteroid.Draw("Model Shader", nrOfAsteroids);
        shaderManager::setInt("drawObject", 0);
        planet.Draw("Model Shader");

        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbms->framebufferID);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb->framebufferID);
        glBlitFramebuffer(0, 0, frameWidth, frameHeight, 0, 0, frameWidth, frameHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        shaderManager::setAndUse("Simple Shader");
        glBindTexture(GL_TEXTURE_2D, fb->textureAttachemntID);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(frontSquareVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        cam->orthogonize();

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
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        cam->instantMove(glm::vec3(0.0f, 2.5f, 10.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
        cam->adjustSpeed(2.0);
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
        cam->adjustSpeed(0.5);
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
    if (vSync)
        desiredFrameLength = 1000.0/mode->refreshRate;
    return chosenMonitor;
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

framebuffer* createFramebuffer(unsigned int nrOfSamplers, unsigned int width, unsigned height)
{
    framebuffer* fb = new framebuffer();
    glGenFramebuffers(1, &fb->framebufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, fb->framebufferID);

    glGenTextures(1, &fb->textureAttachemntID);
    if (nrOfSamplers > 1)
    {
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, fb->textureAttachemntID);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, nrOfSamplers, GL_RGB, width, height, GL_TRUE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, fb->textureAttachemntID, 0);
    } else  {
        glBindTexture(GL_TEXTURE_2D, fb->textureAttachemntID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb->textureAttachemntID, 0);
    }

    glGenRenderbuffers(1, &fb->renderBufferID);
    if (nrOfSamplers > 1)
    {
        glBindRenderbuffer(GL_RENDERBUFFER, fb->renderBufferID);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, nrOfSamplers, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fb->renderBufferID);
    } else {
        glBindRenderbuffer(GL_RENDERBUFFER, fb->renderBufferID);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fb->renderBufferID);
    }
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout<<"Creation of framebuffer failed!"<<std::endl;
        exit(1);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return fb;
}


float getRandom(float left, float right)
{
    float diff = right - left;
    float randVal = (float)rand()/(float)RAND_MAX;
    return(left + diff*randVal);
}
