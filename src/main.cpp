#include <iostream>
#include <string>
#include <math.h>
#include <map>
#include <glad.h>
#include <glfw3.h>
#include <shader.h>
#include <camera.h>
#include <main.h>
#include <textOutput.h>
#include <model.h>
#include <random>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

#include <stb_image.h>

#define my_Pi 3.14159265359
#define my_Pi_2 1.57079632679

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);
float getRandom(float left, float right);
glm::mat3 getNormalMatrix(glm::mat4 modelMatrix);
void printOutMatrix(glm::mat3 mat);
void printOutMatrix(glm::mat4 mat);
void querryGlParams();
GLFWmonitor* getMonitor();
camera* cam;

static int frameWidth = 800;
static int frameHeight = 600;
static int monitor = SECOND_MONITOR;
static bool vSync = true;
static float desiredFrameLength = 1000.0f/60.0f;
static float movementSpeed = 4.5f;

int main()
{
    glfwInit();

    GLFWmonitor *bestMonitor = getMonitor();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(frameWidth, frameHeight, "LearnOpwnGL", bestMonitor, nullptr);
    if (window == nullptr)
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

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    if (!shaderManager::addShadervf("./res/shaders/pbr.vertex.shader", "./res/shaders/pbr.fragment.shader", "PBR"))
        return -1;
    if (!shaderManager::addShadervf("./res/shaders/cubemap.vertex.shader", "./res/shaders/equirectangular_to_cubemap.fragment.shader", "ETC"))
        return -1;
    if (!shaderManager::addShadervf("./res/shaders/cubemap.vertex.shader", "./res/shaders/irradiance_convolution.fragment.shader", "IC"))
        return -1;
    if (!shaderManager::addShadervf("./res/shaders/cubemap.vertex.shader", "./res/shaders/prefilter.fragment.shader", "PF"))
        return -1;
    if (!shaderManager::addShadervf("./res/shaders/brdf.vertex.shader", "./res/shaders/brdf.fragment.shader", "BRDF"))
        return -1;
    if (!shaderManager::addShadervf("./res/shaders/background.vertex.shader", "./res/shaders/background.fragment.shader", "BG"))
        return -1;
	if (!shaderManager::addShadervf("./res/shaders/test.vertex.shader", "./res/shaders/test.fragment.shader", "Test"))
		return -1;

    cam = new camera(glm::vec3(0.0f, 0.0f, 15.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), my_Pi_2/2.0f, 0.1f, 10000.0f, 7.5f, (float)frameWidth, (float)frameHeight, YAW_ROLL_PITCH);

    Model cerberus("./res/models/Cerberus/Cerberus_LP.FBX", false);

	glm::mat4 captureProjection = glm::perspective(my_Pi_2, 1.0, 0.1, 10.0);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	unsigned int cubeVAO, cubeVBO, quadVAO, quadVBO;
	
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glBindVertexArray(cubeVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	unsigned int AlbedoTexture = TextureFromFile("Cerberus_A.tga", "./res/models/Cerberus/Textures", true);
	unsigned int NormalTexture = TextureFromFile("Cerberus_N.tga", "./res/models/Cerberus/Textures", true);
	unsigned int MetallicTexture = TextureFromFile("Cerberus_M.tga", "./res/models/Cerberus/Textures", true);
	unsigned int RoughnessTexture = TextureFromFile("Cerberus_R.tga", "./res/models/Cerberus/Textures", true);
	unsigned int AOTexture = TextureFromFile("Cerberus_O.jpg", "./res/models/Cerberus/Textures", true);

    glViewport(0, 0, frameWidth, frameHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	unsigned int captureFBO;
	unsigned int captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 2048, 2048);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	//Here we load our hdr texture
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrOfComponents;
	float *data = stbi_loadf("./res/textures/Beach.hdr", &width, &height, &nrOfComponents, 0);
	unsigned int hdrTexture;
	if (data)
	{
		glGenTextures(1, &hdrTexture);
		glBindTexture(GL_TEXTURE_2D, hdrTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		std::cout << "Failed to load HDR image." << std::endl;
	}

	//Transform hdr 2D texture to cubemap
	unsigned int envCubemap;
	glGenTextures(1, &envCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 2048, 2048, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	shaderManager::setAndUse("ETC");
	shaderManager::setInt("equirectangularMap", 0);
	shaderManager::setMat4("projection", glm::value_ptr(captureProjection));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);

	glViewport(0, 0, 2048, 2048);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; i++)
	{
		shaderManager::setMat4("view", glm::value_ptr(captureViews[i]));
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	//Calculate the irradiance cubemap
	unsigned int irradianceMap;
	glGenTextures(1, &irradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

	shaderManager::setAndUse("IC");
	shaderManager::setInt("environmentMap", 0);
	shaderManager::setMat4("projection", glm::value_ptr(captureProjection));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

	glViewport(0, 0, 32, 32);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		shaderManager::setMat4("view", glm::value_ptr(captureViews[i]));
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}

	//Calculate prefilter map
	unsigned int prefilterMap;
	glGenTextures(1, &prefilterMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	shaderManager::setAndUse("PF");
	shaderManager::setInt("environmentMap", 0);
	shaderManager::setMat4("projection", glm::value_ptr(captureProjection));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		unsigned int mipWidth = 128 * std::pow(0.5, mip);
		unsigned int mipHeight = 128 * std::pow(0.5, mip);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = float(mip) / float(maxMipLevels - 1);
		shaderManager::setFloat("roughness", roughness);
		for (unsigned int i = 0; i < 6; ++i)
		{
			shaderManager::setMat4("view", glm::value_ptr(captureViews[i]));
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glBindVertexArray(cubeVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	unsigned int brdfLUTTexture;
	glGenTextures(1, &brdfLUTTexture);

	glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 2048, 2048);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

	glViewport(0, 0, 512, 512);
	shaderManager::setAndUse("BRDF");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glm::vec3 lightPositions[] = {
		glm::vec3(-10.0f,  10.0f, 10.0f),
		glm::vec3(10.0f,  10.0f, 10.0f),
		glm::vec3(-10.0f, -10.0f, 10.0f),
		glm::vec3(10.0f, -10.0f, 10.0f),
	};
	glm::vec3 lightColors[] = {
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f)
	};

	shaderManager::setAndUse("PBR");
	shaderManager::setInt("irradianceMap", 0);
	shaderManager::setInt("prefilterMap", 1);
	shaderManager::setInt("brdfLUT", 2);
	shaderManager::setInt("albedoMap", 3);
	shaderManager::setInt("normalMap", 4);
	shaderManager::setInt("metallicMap", 5);
	shaderManager::setInt("roughnessMap", 6);
	shaderManager::setInt("aoMap", 7);
	shaderManager::setAndUse("BG");
	shaderManager::setInt("environmentMap", 0);

	glViewport(0, 0, frameWidth, frameHeight);

    static float deltaT = 0.0f;
    static float currentFrame = 0.0f;
    static float previousFrame = 0.0f;

	std::cout << "Entering game loop" << std::endl;

    while (!glfwWindowShouldClose(window))
    {
        currentFrame = glfwGetTime();
        deltaT = currentFrame - previousFrame;
        previousFrame = currentFrame;

        processInput(window);

		glm::mat4 projection = cam->getProjection();
        glm::mat4 view = cam->getViewMatrix();
        glm::vec3 camPos = cam->getPosition();
        glm::mat4 model(1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderManager::setAndUse("PBR");
		shaderManager::setMat4("projection", glm::value_ptr(projection));
        shaderManager::setMat4("view", glm::value_ptr(view));
		model = glm::translate(model, glm::vec3(-5.0, 0.0, 0.0));
		model = glm::rotate(model, float(-my_Pi_2), glm::vec3(1.0, 0.0, 0.0f));
		model = glm::rotate(model, float(my_Pi_2), glm::vec3(0.0, 0.0, 1.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		shaderManager::setMat4("model", glm::value_ptr(model));
		glm::mat3 normal = getNormalMatrix(model);
		shaderManager::setMat3("normal", glm::value_ptr(normal));
		shaderManager::setVec3("camPos", glm::value_ptr(camPos));

		for (unsigned int i = 0; i < 5; i++)
		{
			shaderManager::setVec3("lightColors[" + std::to_string(i) + "]", glm::value_ptr(lightColors[i]));
			shaderManager::setVec3("lightPositions[" + std::to_string(i) + "]", glm::value_ptr(lightPositions[i]));
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, AlbedoTexture);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, NormalTexture);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, MetallicTexture);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, RoughnessTexture);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, AOTexture);

        cerberus.DrawTextureless("PBR");

		shaderManager::setAndUse("BG");
		shaderManager::setMat4("view", glm::value_ptr(view));
		shaderManager::setMat4("projection", glm::value_ptr(projection));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

        cam->orthogonize();

        Sleep(15);
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

    currentInput = float(glfwGetTime());
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
    static double mouseSensitivity = 0.0005;
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

glm::mat3 getNormalMatrix(glm::mat4 modelMatrix)
{
    return glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
}

void printOutMatrix(glm::mat3 mat)
{
    for (unsigned int i = 0; i < 3; i++)
        std::cout<<mat[i][0]<<" "<<mat[i][1]<<" "<<mat[i][2]<<std::endl;
    std::cout<<std::endl;
}

void printOutMatrix(glm::mat4 mat)
{
    for (unsigned int i = 0; i < 4; i++)
        std::cout<<mat[i][0]<<" "<<mat[i][1]<<" "<<mat[i][2]<<" "<<mat[i][3]<<std::endl;
    std::cout<<std::endl;
}
