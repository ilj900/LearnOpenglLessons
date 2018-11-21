#include <iostream>
#include <string>
#include <math.h>
#include <map>
#include "glad.h"
#include "glfw3.h"
#include "shader.h"
#include "camera.h"
#include "textOutput.h"

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
void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);
void querryGlParams();
unsigned int loadTexture(const char * path);
GLFWmonitor* getMonitor();
camera *cam;
bool spotlightSwitchOn = false;

#include "main.h"

static int frameWidth = 800;
static int frameHeight = 600;
static int monitor = SECOND_MONITOR;
static bool vSync = true;
static float desiredFrameLength = 1000.0f/60.0f;

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
    if (!shaderManager::addShadervf("./res/shaders/text.vertex.shader", "./res/shaders/text.fragment.shader", "Text Shader"))
        return -1;
    if (!shaderManager::addShadervf("./res/shaders/common.vertex.shader", "./res/shaders/common.fragment.shader", "Common Shader"))
        return -1;

    cam = new camera(glm::vec3(0.0f, 2.5f, 10.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), M_PI_2/2.0f, 0.1f, 100.0f, 4.5f, (float)frameWidth, (float)frameHeight);
    CHARSET *chrst = setupTreeType("fonts/arial.ttf", 64, 128);    /// I found out that symbols greater that 128 loads blank;

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
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    int numberOfFramesInFrameLengthHistory = frameWidth*0.5;
    int currentFrameNumber = 0;
    float *innitialValues = new float[numberOfFramesInFrameLengthHistory*2]();  ///3600 of (x, y)
    for (int i = 0; i<numberOfFramesInFrameLengthHistory; i++)
        innitialValues[2*i] = i;
    unsigned int frameLengthVBO, frameLengthVAO;
    glGenBuffers(1, &frameLengthVBO);
    glGenVertexArrays(1, &frameLengthVAO);
    glBindVertexArray(frameLengthVAO);
    glBindBuffer(GL_ARRAY_BUFFER, frameLengthVBO);
    glBufferData(GL_ARRAY_BUFFER, numberOfFramesInFrameLengthHistory*2*sizeof(float), innitialValues, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    delete[] innitialValues;

    unsigned int frameLengthMarkupVBO, frameLengthMarkupVAO;
    glGenBuffers(1, &frameLengthMarkupVBO);
    glGenVertexArrays(1, &frameLengthMarkupVAO);
    glBindVertexArray(frameLengthMarkupVAO);
    glBindBuffer(GL_ARRAY_BUFFER, frameLengthMarkupVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(frameLengthMarkup), frameLengthMarkup, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    unsigned int gridVBO, gridVAO;
    glGenBuffers(1, &gridVBO);
    glGenVertexArrays(1, &gridVAO);
    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lineGrid), lineGrid, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    unsigned int textureDiffuse = loadTexture("./res/textures/container2.png");
    unsigned int textureSpecular = loadTexture("./res/textures/container2_specular.png");

    glm::vec3 gridColor(0.0f, 1.0f, 0.0f);
    static float deltaT = 0.0f;
    static float currentFrame = 0.0f;
    static float previousFrame = 0.0f;
    static float yetOneMoreTimer = 0.0f;
    glm::mat4 orthoProjection = glm::ortho(0.0f, (float)frameWidth, 0.0f, (float)frameHeight);
    glm::vec2 frameLengthGraphPos = glm::vec2(0.0f, 0.0f);
    glm::vec3 lineColor = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 lightSourceColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 backgroundLineColor = glm::vec3(0.0f, 1.0f, 1.0f);
    glm::vec2 frameLengthScale = glm::vec2(2.0f, 16.0f);                        ///You can note that I stretch the actual values of frame length by 16
    glm::vec2 frameLengthMarkupScale = glm::vec2((float)frameWidth, 80.0f);     ///and the line of markup by 80, so the first line indicates 5, second 10, third 15 and fourth 20

    TEXT* smlMssg = generateTextData("Hell, It's about time!", chrst, 10);
    TEXT* FPStxt = generateTextData("FPS: ", chrst, 10);
    TEXT **digitsArray = new TEXT*[10]();
    for (unsigned int i = 0; i < 10; i++)
        digitsArray[i] = generateTextData(std::to_string(i), chrst, 10);
    TEXT* dot = generateTextData(".", chrst, 10);

    yetOneMoreTimer = glfwGetTime();
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

        shaderManager::setAndUse("Common Shader");
        shaderManager::setInt("targetFlag", SPACE_GRID);
        shaderManager::setMat4("model", glm::value_ptr(model));
        shaderManager::setMat4("view", glm::value_ptr(view));
        shaderManager::setMat4("projection", glm::value_ptr(projection));
        shaderManager::setVec3("generalColor", glm::value_ptr(gridColor));
        glBindVertexArray(gridVAO);
        glDrawArrays(GL_LINES, 0, 44);
        glBindVertexArray(0);

        shaderManager::setInt("targetFlag", SIMPLE_LINES);
        shaderManager::setVec2("scale", glm::value_ptr(frameLengthMarkupScale));
        shaderManager::setVec3("generalColor", glm::value_ptr(backgroundLineColor));
        shaderManager::setMat4("projection", glm::value_ptr(orthoProjection));
        glBindVertexArray(frameLengthMarkupVAO);
        glDrawArrays(GL_LINES, 0, 8);
        glBindVertexArray(0);

        shaderManager::setInt("targetFlag", LIGHT_SHAPE);
        shaderManager::setMat4("projection", glm::value_ptr(projection));
        shaderManager::setMat4("view", glm::value_ptr(view));
        shaderManager::setVec3("generalColor", glm::value_ptr(lightSourceColor));
        glBindVertexArray(lightVAO);
        for(int i = 0; i < 4; i++)
        {
            model = glm::mat4(1.0);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.05f));
            shaderManager::setMat4("model", glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);

        float deltaMsT = 1000.0f * deltaT;
        static bool drawSecondPart = false;
        glBindBuffer(GL_ARRAY_BUFFER, frameLengthVBO);
        glBufferSubData(GL_ARRAY_BUFFER, (currentFrameNumber * 2 + 1) * sizeof(float), sizeof(float), &deltaMsT);   /// currentFrameNumber * 2 + 1 points at the 'y'
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        /// Now we have updated our "fifo" buffer. The only trick now is to draw it apropriately
        glBindVertexArray(frameLengthVAO);
        shaderManager::setInt("targetFlag", FPS_GRAPH);
        shaderManager::setFloat("offsetX", (float)currentFrameNumber);
        shaderManager::setMat4("projection", glm::value_ptr(orthoProjection));
        shaderManager::setVec2("pos", glm::value_ptr(frameLengthGraphPos));
        shaderManager::setFloat("totalWidth", (float)numberOfFramesInFrameLengthHistory);
        shaderManager::setVec2("scale", glm::value_ptr(frameLengthScale));
        shaderManager::setVec3("generalColor", glm::value_ptr(lineColor));
        glPointSize(2.0f);
        static int skipFirstdeltaFrame = 1; ///I'm skipping first frame cause it shows big frame length;
        glDrawArrays(GL_LINE_STRIP, skipFirstdeltaFrame, std::max(currentFrameNumber-skipFirstdeltaFrame, 0));
        if(drawSecondPart)
        {
            glDrawArrays(GL_LINE_STRIP, currentFrameNumber, std::max(numberOfFramesInFrameLengthHistory-currentFrameNumber-1, 0));
            skipFirstdeltaFrame = 0;
        }
        glPointSize(1.0f);
        glBindVertexArray(0);

        shaderManager::setAndUse("Simple Shader");
        glBindVertexArray(VAO);
        shaderManager::setInt("material.diffuse", 0);
        shaderManager::setInt("material.specular", 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureDiffuse);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textureSpecular);
        shaderManager::setVec3("viewPos", glm::value_ptr(camPos));
        shaderManager::setFloat("material.shininess", 32.0f);
        shaderManager::setMat4("view", glm::value_ptr(view));
        shaderManager::setMat4("projection", glm::value_ptr(projection));
        shaderManager::setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        shaderManager::setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        shaderManager::setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        shaderManager::setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        // point light 1
        shaderManager::setVec3("pointLights[0].position", glm::value_ptr(pointLightPositions[0]));
        shaderManager::setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        shaderManager::setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        shaderManager::setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        shaderManager::setFloat("pointLights[0].constant", 1.0f);
        shaderManager::setFloat("pointLights[0].linear", 0.09);
        shaderManager::setFloat("pointLights[0].quadratic", 0.032);
        // point light 2
        shaderManager::setVec3("pointLights[1].position", glm::value_ptr(pointLightPositions[1]));
        shaderManager::setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        shaderManager::setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        shaderManager::setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        shaderManager::setFloat("pointLights[1].constant", 1.0f);
        shaderManager::setFloat("pointLights[1].linear", 0.09);
        shaderManager::setFloat("pointLights[1].quadratic", 0.032);
        // point light 3
        shaderManager::setVec3("pointLights[2].position", glm::value_ptr(pointLightPositions[2]));
        shaderManager::setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        shaderManager::setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        shaderManager::setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        shaderManager::setFloat("pointLights[2].constant", 1.0f);
        shaderManager::setFloat("pointLights[2].linear", 0.09);
        shaderManager::setFloat("pointLights[2].quadratic", 0.032);
        // point light 4
        shaderManager::setVec3("pointLights[3].position", glm::value_ptr(pointLightPositions[3]));
        shaderManager::setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        shaderManager::setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        shaderManager::setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        shaderManager::setFloat("pointLights[3].constant", 1.0f);
        shaderManager::setFloat("pointLights[3].linear", 0.09);
        shaderManager::setFloat("pointLights[3].quadratic", 0.032);
        // spotLight
        shaderManager::setBool("spotlightSwitch", spotlightSwitchOn);
        if (spotlightSwitchOn)
        {
            shaderManager::setVec3("spotLight.position", glm::value_ptr(camPos));
            shaderManager::setVec3("spotLight.direction", glm::value_ptr(camDir));
            shaderManager::setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
            shaderManager::setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
            shaderManager::setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
            shaderManager::setFloat("spotLight.constant", 1.0f);
            shaderManager::setFloat("spotLight.linear", 0.09);
            shaderManager::setFloat("spotLight.quadratic", 0.032);
            shaderManager::setFloat("spotLight.innerAngle", glm::cos(glm::radians(12.5f)));
            shaderManager::setFloat("spotLight.outerAngle", glm::cos(glm::radians(15.0f)));
        }
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
        glBindVertexArray(0);

        renderText("Text Shader", smlMssg, 10.0, (float)frameHeight - smlMssg->height - 10.0, 1.0, glm::vec3(0.859375f, 0.078125f, 0.234375f), frameWidth, frameHeight);
        renderText("Text Shader", FPStxt, 10.0, (float)frameHeight - FPStxt->height - smlMssg->height - 10.0, 1.0, glm::vec3(0.859375f, 0.078125f, 0.234375f), frameWidth, frameHeight);
        float offsetX = 10 + FPStxt->width;
        float offsetY = (float)frameHeight - FPStxt->height - smlMssg->height - 10.0;
        char buff[50];
        std::sprintf(buff, "%i", (int)(1000.0f/deltaMsT));
        std::string fpsStr(buff);
        for (unsigned int i = 0; i < fpsStr.size(); i++)
        {
            char c = fpsStr[i];
            if (c=='.')
            {
                renderText("Text Shader", dot, offsetX, offsetY, 1.0, glm::vec3(0.859375f, 0.078125f, 0.234375f), frameWidth, frameHeight);
                offsetX+=dot->width;
            }
            int index = c - '0';
            if (index >= 0 && index <= 9)
            {
                renderText("Text Shader", digitsArray[index], offsetX, offsetY, 1.0, glm::vec3(0.859375f, 0.078125f, 0.234375f), frameWidth, frameHeight);
                offsetX+=digitsArray[index]->width;
            }
        }

        currentFrameNumber++;
        if(currentFrameNumber >= numberOfFramesInFrameLengthHistory)
        {
            currentFrameNumber = 0;
            drawSecondPart = true;
        }
        yetOneMoreTimer += desiredFrameLength;
        static float timeToSleep = (yetOneMoreTimer - glfwGetTime());
        Sleep((int)timeToSleep);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &gridVAO);
    glDeleteVertexArrays(1, &frameLengthVAO);
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

