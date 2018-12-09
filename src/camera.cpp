#include <iostream>
#include <camera.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

static float MY_PI = 3.14159265359;
static float MY_PI_2 = 3.14159265359/2.0;

camera::camera(glm::vec3 position, glm::vec3 front, glm::vec3 up, glm::vec3 right, float fov, float fPlane, float bPlane, float speed, float screenWidth, float screenHeight, movementOrder mvO) :
    cameraPos(position),
    cameraFront(front),
    cameraUp(up),
    cameraRight(right),
    speed(speed),
    fov(fov),
    nearClipPlane(fPlane),
    farClipPlane(bPlane),
    screenWidth(screenWidth),
    screenHeight(screenHeight),
    mvOrder(mvO)
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
    screenHeight(1080.0f),
    mvOrder(ROLL_PITCH_YAW)
{
}

void camera::rotate(float yaw, float pitch, float roll)
{
    glm::quat yawQuat = glm::angleAxis(yaw, cameraUp);
    glm::quat pitchQuat = glm::angleAxis(pitch, cameraRight);
    glm::quat rollQuat = glm::angleAxis(roll, cameraFront);
    switch (mvOrder)
    {
    case PITCH_ROLL_YAW:
        cameraUp = rollQuat * pitchQuat * cameraUp * glm::inverse(pitchQuat) * glm::inverse(rollQuat);
        cameraRight = yawQuat * rollQuat * cameraRight * glm::inverse(rollQuat) * glm::inverse(yawQuat);
        cameraFront = yawQuat * pitchQuat * cameraFront * glm::inverse(pitchQuat) * glm::inverse(yawQuat);
        break;
    case PITCH_YAW_ROLL:
        cameraUp = rollQuat * pitchQuat * cameraUp * glm::inverse(pitchQuat) * glm::inverse(rollQuat);
        cameraRight = rollQuat * yawQuat * cameraRight * glm::inverse(yawQuat) * glm::inverse(rollQuat);
        cameraFront = yawQuat * pitchQuat * cameraFront * glm::inverse(pitchQuat) * glm::inverse(yawQuat);
        break;
    case YAW_ROLL_PITCH:
        cameraUp = pitchQuat * rollQuat * cameraUp * glm::inverse(rollQuat) * glm::inverse(pitchQuat);
        cameraRight = rollQuat * yawQuat * cameraRight * glm::inverse(yawQuat) * glm::inverse(rollQuat);
        cameraFront = pitchQuat * yawQuat * cameraFront * glm::inverse(yawQuat) * glm::inverse(pitchQuat);
        break;
    case YAW_PITCH_ROLL:
        cameraUp = rollQuat * pitchQuat * cameraUp * glm::inverse(pitchQuat) * glm::inverse(rollQuat);
        cameraRight = rollQuat * yawQuat * cameraRight * glm::inverse(yawQuat) * glm::inverse(rollQuat);
        cameraFront = pitchQuat * yawQuat * cameraFront * glm::inverse(yawQuat) * glm::inverse(pitchQuat);
        break;
    case ROLL_PITCH_YAW:
        cameraUp = pitchQuat * rollQuat * cameraUp * glm::inverse(rollQuat) * glm::inverse(pitchQuat);
        cameraRight = yawQuat * rollQuat * cameraRight * glm::inverse(rollQuat) * glm::inverse(yawQuat);
        cameraFront = yawQuat * pitchQuat * cameraFront * glm::inverse(pitchQuat) * glm::inverse(yawQuat);
        break;
    case ROLL_YAW_PITCH:
        cameraUp = pitchQuat * rollQuat * cameraUp * glm::inverse(rollQuat) * glm::inverse(pitchQuat);
        cameraRight = yawQuat * rollQuat * cameraRight * glm::inverse(rollQuat) * glm::inverse(yawQuat);
        cameraFront = pitchQuat * yawQuat * cameraFront * glm::inverse(yawQuat) * glm::inverse(pitchQuat);
        break;
    }
    cameraUp = glm::normalize(cameraUp);
    cameraRight = glm::normalize(cameraRight);
    cameraFront = glm::normalize(cameraFront);
}

void camera::move(float dForward, float dRight, float dUp)
{
    cameraPos += cameraFront*dForward*speed;
    cameraPos += cameraRight*dRight*speed;
    cameraPos += cameraUp*dUp*speed;
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

void camera::orthogonize()
{
    glm::vec3 up = glm::normalize(glm::cross(cameraRight, cameraFront));
    glm::vec3 right = glm::normalize(glm::cross(cameraFront, up));
    cameraFront = glm::normalize(cameraFront);
    cameraUp = up;
    cameraRight = right;
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
