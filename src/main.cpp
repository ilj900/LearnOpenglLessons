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
camera *cam;
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

    cam = new camera(glm::vec3(0.0f, 0.5f, 50.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), M_PI_2/2.0f, 0.1f, 1000.0f, 7.5f, (float)frameWidth, (float)frameHeight);

    glViewport(0, 0, frameWidth, frameHeight);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Model myModel("J:/Workspace/Models/deathwing/deathwing.obj");
    std::cout<<"Loaded"<<std::endl;

    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
    glBindVertexArray(0);

    unsigned int objectVAO, objectVBO;
    glGenVertexArrays(1, &objectVAO);
    glGenBuffers(1, &objectVBO);
    glBindVertexArray(objectVAO);
    glBindBuffer(GL_ARRAY_BUFFER, objectVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(objectsPositions), objectsPositions, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    std::vector<glm::vec3> objects
    {
        glm::vec3(-1.5f, 0.0f, -0.48f),
        glm::vec3( 1.5f, 0.0f, 0.51f),
        glm::vec3( 0.0f, 0.0f, 0.7f),
        glm::vec3(-0.3f, 0.0f, -2.3f),
        glm::vec3 (0.5f, 0.0f, -0.6f)
    };

    unsigned int grassTexture = TextureFromFile("grass.png", "./res/textures", false);
    unsigned int metalTexture = TextureFromFile("metal.png", "./res/textures", false);
    unsigned int windowTexture = TextureFromFile("window.png", "./res/textures", false);

    static float deltaT = 0.0f;
    static float currentFrame = 0.0f;
    static float previousFrame = 0.0f;

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

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDisable(GL_BLEND);
        shaderManager::setAndUse("Model Shader");
        shaderManager::setMat4("projection", glm::value_ptr(projection));
        shaderManager::setMat4("view", glm::value_ptr(view));

        for(unsigned int i = 0; i < NUMBER_OF_MODELS; i++)
        {
            model = glm::mat4(1.0);
            model = glm::translate(model, model_positions[i]);
            model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
            shaderManager::setMat4("model", glm::value_ptr(model));
            myModel.Draw("Model shader");
        }

        shaderManager::setAndUse("Common Shader");
        shaderManager::setMat4("projection", glm::value_ptr(projection));
        shaderManager::setMat4("view", glm::value_ptr(view));
        model = glm::mat4(1.0f);
        shaderManager::setMat4("model", glm::value_ptr(model));
        shaderManager::setInt("texture1", 0);
        shaderManager::setBool("drawGrass", true);
        glBindVertexArray(planeVAO);
        glBindTexture(GL_TEXTURE_2D, metalTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glBindVertexArray(objectVAO);
        glBindTexture(GL_TEXTURE_2D, grassTexture);
        for(unsigned int i = 0; i < objects.size(); i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, objects[i]);
            model = glm::translate(model, glm::vec3(0.0, 0.0, 35.0));
            shaderManager::setMat4("model", glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        std::map<float, glm::vec3> sortedWindows;
        for (unsigned int i = 0; i < objects.size(); i++)
        {
            float sqrDist = glm::distance2(camPos, objects[i]);
            sortedWindows[sqrDist] = objects[i];
        }
        glEnable(GL_BLEND);
        shaderManager::setBool("drawGrass", false);
        glBindTexture(GL_TEXTURE_2D, windowTexture);
        for(std::map<float, glm::vec3>::reverse_iterator it = sortedWindows.rbegin(); it != sortedWindows.rend(); ++it)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, it->second);
            model = glm::translate(model, glm::vec3(0.0, 0.0, 45.0));
            shaderManager::setMat4("model", glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &objectVAO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &objectVBO);
    glDeleteBuffers(1, &planeVBO);

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


float getRandom(float left, float right)
{
    float diff = right - left;
    float randVal = (float)rand()/(float)RAND_MAX;
    return(left + diff*randVal);
}
