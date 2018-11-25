#include <iostream>
#include <camera.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

static float MY_PI = 3.14159265359;
static float MY_PI_2 = 3.14159265359/2.0;

camera::camera(glm::vec3 position, glm::vec3 front, glm::vec3 up, glm::vec3 right, float fov, float fPlane, float bPlane, float speed, float screenWidth, float screenHeight) :
    cameraPos(position),
    cameraFront(front),
    cameraUp(up),
    cameraRight(right),
    speed(speed),
    fov(fov),
    nearClipPlane(fPlane),
    farClipPlane(bPlane),
    screenWidth(screenWidth),
    screenHeight(screenHeight)
{
}

camera::camera() :
    cameraPos(glm::vec3(0.0f, 0.0f, 2.0f)),
    cameraFront(glm::vec3(0.0f, 0.0f, -1.0f)),
    cameraUp(glm::vec3(0.0f, 1.0f, 0.0f)),
    cameraRight(1.0f, 0.0f, 0.0f),
    speed(2.5f),
    fov(M_PI_2),
    nearClipPlane(1.0f),
    farClipPlane(100.0f),
    screenWidth(1920.0f),
    screenHeight(1080.0f)
{
}

void camera::rotate(float yaw, float pitch, float roll)
{
    glm::quat yawQuat = glm::angleAxis(yaw, cameraUp);
    glm::quat pitchQuat = glm::angleAxis(pitch, cameraRight);
    glm::quat rollQuat = glm::angleAxis(roll, cameraFront);
    /// Yaw->>Pitch->>Roll ///Experiment here with order
    cameraUp = rollQuat * pitchQuat * cameraUp * glm::inverse(pitchQuat) * glm::inverse(rollQuat);
    cameraRight = yawQuat * rollQuat * cameraRight * glm::inverse(rollQuat) * glm::inverse(yawQuat);
    cameraFront = yawQuat * pitchQuat * cameraFront * glm::inverse(pitchQuat) * glm::inverse(yawQuat);
    cameraUp = glm::normalize(cameraUp);
    cameraRight = glm::normalize(cameraRight);
    cameraFront = glm::normalize(cameraFront);
}

void camera::move(float dForward, float dRight, float dUp)
{
    cameraPos += cameraFront*dForward;
    cameraPos += cameraRight*dRight;
    cameraPos += cameraUp*dUp;
}

void camera::instantMove(glm::vec3 position, glm::vec3 front, glm::vec3 up, glm::vec3 right)
{
    cameraPos = position;
    cameraFront = front;
    cameraUp = up;
    cameraRight = right;
}

void camera::zoom(float d)
{
    fov+=d*0.01f;
    if(fov > 0.99f*M_PI)
        fov = 0.99f*M_PI;
    if (fov < 0.01f*M_PI)
        fov = 0.01f*M_PI;
}

glm::mat4 camera::getViewMatrix()
{
    return glm::lookAt(cameraPos, cameraPos+cameraFront, cameraUp);
}

glm::mat4 camera::getProjection()
{
    return glm::perspective(fov, screenWidth/screenHeight, nearClipPlane, farClipPlane);
}

glm::vec3 camera::getPosition()
{
    return cameraPos;
}

glm::vec3 camera::getDirection()
{
    return cameraFront;
}
