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
float getRandom(float left, float right);
void querryGlParams();
GLFWmonitor* getMonitor();
#define NUMBER_OF_CAMERAS 6
camera* cam[NUMBER_OF_CAMERAS];
bool spotlightSwitchOn = false;
unsigned int currentHighlighttedDragon = 0;

#include <main.h>

static int frameWidth = 800;
static int frameHeight = 600;
static int monitor = SECOND_MONITOR;
static bool vSync = true;
static float desiredFrameLength = 1000.0f/60.0f;

#define NUMBER_OF_MODELS 30
glm::vec3 model_positions[NUMBER_OF_MODELS];

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

    for (unsigned int i = 0; i < NUMBER_OF_MODELS; i++)
        model_positions[i] = glm::vec3(getRandom(-30.0f, 30.0f), -0.5, getRandom(-30.0f, 30.0f));


    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!shaderManager::addShadervf("./res/shaders/model.vertex.shader", "./res/shaders/model.fragment.shader", "Model Shader"))
        return -1;
    if (!shaderManager::addShadervf("./res/shaders/common.vertex.shader", "./res/shaders/common.fragment.shader", "Common Shader"))
        return -1;

    movementOrder whyDoINeedThisArray[NUMBER_OF_CAMERAS] = {ROLL_PITCH_YAW, ROLL_YAW_PITCH, PITCH_YAW_ROLL, PITCH_ROLL_YAW,  YAW_PITCH_ROLL, YAW_ROLL_PITCH};
    for (unsigned int i = 0; i < NUMBER_OF_CAMERAS; i++)
        cam[i] = new camera(glm::vec3(0.0f, 0.5f, 50.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), M_PI_2/2.0f, 0.1f, 1000.0f, 7.5f, (float)frameWidth, (float)frameHeight, whyDoINeedThisArray[i]);

    glViewport(0, 0, frameWidth, frameHeight);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    framebuffer* fbo[NUMBER_OF_CAMERAS];
    for (unsigned int i = 0; i < NUMBER_OF_CAMERAS; i++)
    {
        fbo[i] = new framebuffer();
        glGenFramebuffers(1, &fbo[i]->framebufferID);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo[i]->framebufferID);

        glGenTextures(1, &fbo[i]->textureAttachemntID);
        glBindTexture(GL_TEXTURE_2D, fbo[i]->textureAttachemntID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frameWidth, frameHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo[i]->textureAttachemntID, 0);

        glGenRenderbuffers(1, &fbo[i]->renderBufferID);
        glBindRenderbuffer(GL_RENDERBUFFER, fbo[i]->renderBufferID);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, frameWidth, frameHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fbo[i]->renderBufferID);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout<<"Creation of framebuffer failed!"<<std::endl;
            exit(1);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    unsigned int screenQuadVBO, screenQuadVAO;
    glGenVertexArrays(1, &screenQuadVAO);
    glGenBuffers(1, &screenQuadVBO);
    glBindVertexArray(screenQuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenQuads), screenQuads, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) (2 * sizeof(float)));
    glBindVertexArray(0);

    Model myModel("J:/Workspace/Models/deathwing/deathwing.obj");
    std::cout<<"Loaded"<<std::endl;

    shaderManager::setAndUse("Common shader");
    shaderManager::setInt("screenTexture", 0);

    static float deltaT = 0.0f;
    static float currentFrame = 0.0f;
    static float previousFrame = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        currentFrame = glfwGetTime();
        deltaT = currentFrame - previousFrame;
        previousFrame = currentFrame;

        processInput(window);

        glm::mat4 projection[NUMBER_OF_CAMERAS];
        glm::mat4 view[NUMBER_OF_CAMERAS];
        glm::mat4 model[NUMBER_OF_CAMERAS];

        for (unsigned int i = 0; i < NUMBER_OF_CAMERAS; i++)
        {
            projection[i] = cam[i]->getProjection();
            view[i] = cam[i]->getViewMatrix();
            model[i] = glm::mat4(1.0f);
        }

        for(unsigned int i = 0; i < NUMBER_OF_CAMERAS; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, fbo[i]->framebufferID);
            glEnable(GL_DEPTH_TEST);

            glClearColor(0.05f * i, 0.05f * i, 0.05f * i, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            shaderManager::setAndUse("Model Shader");
            shaderManager::setMat4("projection", glm::value_ptr(projection[i]));
            shaderManager::setMat4("view", glm::value_ptr(view[i]));
            model[i] = glm::translate(model[i], glm::vec3(0.0f, -1.75f, 0.0f));
            model[i] = glm::scale(model[i], glm::vec3(0.2f, 0.2f, 0.2f));
            shaderManager::setMat4("model", glm::value_ptr(model[i]));
            myModel.Draw("Model shader");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        float camPosAbsDelta = glm::distance2(cam[0]->getPosition(), cam[5]->getPosition());
        std::cout<<"Delta: " <<camPosAbsDelta<<std::endl;

        shaderManager::setAndUse("Common Shader");
        glBindVertexArray(screenQuadVAO);
        for(unsigned int i = 0; i < NUMBER_OF_CAMERAS; i++)
        {
            glBindTexture(GL_TEXTURE_2D, fbo[i]->textureAttachemntID);
            glDrawArrays(GL_TRIANGLES, NUMBER_OF_CAMERAS*i, NUMBER_OF_CAMERAS);
        }
        glBindVertexArray(0);
        for(unsigned int i = 0; i < NUMBER_OF_CAMERAS; i++)
            cam[i]->orthogonize();

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
    for(unsigned int i = 0; i < NUMBER_OF_CAMERAS/2; i++)
        cam[i]->move(front, right, up);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        for(unsigned int i = 0; i < NUMBER_OF_CAMERAS; i++)
            cam[i]->rotate(0.0f, 0.0f, deltaT * movementSpeed*0.2f);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        for(unsigned int i = 0; i < NUMBER_OF_CAMERAS; i++)
            cam[i]->rotate(0.0f, 0.0f, -deltaT * movementSpeed*0.2f);
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        for(unsigned int i = 0; i < NUMBER_OF_CAMERAS; i++)
            cam[i]->instantMove(glm::vec3(0.0f, 2.5f, 10.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    for(unsigned int i = NUMBER_OF_CAMERAS/2; i < NUMBER_OF_CAMERAS; i++)
        cam[i]->move(front, right, up);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if(key == GLFW_KEY_F && action == GLFW_PRESS)
        spotlightSwitchOn = !spotlightSwitchOn;
    if(key == GLFW_KEY_UP && action == GLFW_PRESS)
        currentHighlighttedDragon+=10;
    if(key == GLFW_KEY_DOWN && action == GLFW_PRESS)
        currentHighlighttedDragon-=10;
    if(key == GLFW_KEY_LEFT && action == GLFW_PRESS)
        currentHighlighttedDragon-=1;
    if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
        currentHighlighttedDragon+=1;
    if (currentHighlighttedDragon < 0)
        currentHighlighttedDragon += NUMBER_OF_MODELS;
    if(currentHighlighttedDragon >= NUMBER_OF_MODELS)
        currentHighlighttedDragon -= NUMBER_OF_MODELS;
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
    for(unsigned int i = 0; i < NUMBER_OF_CAMERAS; i++)
        cam[i]->rotate(deltax, deltay, 0.0f);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    for(unsigned int i = 0; i < NUMBER_OF_CAMERAS; i++)
        cam[i]->zoom(yoffset);
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


float getRandom(float left, float right)
{
    float diff = right - left;
    float randVal = (float)rand()/(float)RAND_MAX;
    return(left + diff*randVal);
}
