#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

enum movementOrder {ROLL_PITCH_YAW, ROLL_YAW_PITCH, PITCH_YAW_ROLL, PITCH_ROLL_YAW,  YAW_PITCH_ROLL, YAW_ROLL_PITCH};

class camera
{
private:
    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;
    glm::vec3 cameraRight;
    float fov;
    float nearClipPlane;
    float farClipPlane;
    float speed;
    float screenWidth;
    float screenHeight;
    movementOrder mvOrder;

public:
    camera();
    camera(glm::vec3 position, glm::vec3 front, glm::vec3 up, glm::vec3 right, float fov, float fPlane, float bPlane, float speed, float screenWidth, float screenHeight, movementOrder mvO);
    void rotate(float yaw, float pitch, float roll);
    void move(float dForward, float dRight, float dUp);
    void instantMove(glm::vec3 position, glm::vec3 front, glm::vec3 up, glm::vec3 right);
    void zoom(float d);
    glm::mat4 getViewMatrix();
    glm::mat4 getProjection();
    glm::vec3 getPosition();
    glm::vec3 getDirection();
    void adjustSpeed(float value);
    void orthogonize(); /// Call this function to make front up and right vectors perpendecular
};

#endif
